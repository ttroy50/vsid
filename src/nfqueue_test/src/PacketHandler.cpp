#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

extern "C" 
{
	#include <linux/netfilter.h>  /* Defines verdicts (NF_ACCEPT, etc) */
	#include <libnetfilter_queue/libnetfilter_queue.h>
}

#include <iostream>
#include <iomanip>
#include <exception>
#include <time.h>


#include "PacketHandler.h"
#include "Constants.h"
#include "TcpIpv4.h"
#include "UdpIpv4.h"
#include "Logger.h"
#include "StringException.h"

#include "Config.h"

using namespace std;
using namespace Vsid;
using namespace VsidNetfilter;
using namespace VsidCommon;

/* Definition of callback function */
static int nfqPacketHandlerCb(struct nfq_q_handle* nfQueue, 
				struct nfgenmsg *msg,
            	struct nfq_data *pkt, 
            	void *data)
{
    int verdict = NF_ACCEPT;
    PacketHandler* handler = static_cast<PacketHandler*>(data);

    uint32_t id = 0;
	nfqnl_msg_packet_hdr *header = nfq_get_msg_packet_hdr(pkt);

	if (header) 
	{
		id = ntohl(header->packet_id);
	}

    if(data == NULL)
    {
    	SLOG_ERROR(<< "packet handler is NULL");
    	return handler->setVerdictLocal(id, verdict); 
    }
    
    if ( !handler->handlePacket(nfQueue, msg, pkt) )
    {
    	SLOG_ERROR( << "Verdict not set in handlePacket")
    	// use handler setVerdict to increas stats
    	//return handler->setVerdict(id, verdict); 
    }
    return 1;
}

PacketHandler::PacketHandler(int queueNumber ) :
	_shutdown(false),
	_numPackets(0),
	_verdictStats(NF_MAX_VERDICT, 0)
{
	_queueNumber = queueNumber;
	_nfqHandle = NULL;
	_nfQueue = NULL;
	_netlinkHandle = NULL;

	// Get a queue connection handle from the module
	if (!(_nfqHandle = nfq_open())) 
	{
		SLOG_ERROR( << "Error in nfq_open()" );
		throw StringException("Error in nfq_open()");
	}

	// Unbind the handler from processing any IP packets
	if (nfq_unbind_pf(_nfqHandle, AF_INET) < 0) 
	{
		SLOG_ERROR( << "Error in nfq_unbind_pf()" );
		throw StringException("Error in nfq_unbind_pf()");
	}

	// Bind this handler to process IP packets...
	// We only care about IPv4 in initial version
	if (nfq_bind_pf(_nfqHandle, AF_INET) < 0) 
	{
		SLOG_ERROR( << "Error in nfq_bind_pf()" );
		throw StringException("Error in nfq_bind_pf()");
	}

	_nfQueue = nfq_create_queue(_nfqHandle, _queueNumber, &nfqPacketHandlerCb, static_cast<void*>(this) );
	if (_nfQueue == NULL) 
	{
		SLOG_ERROR( << "Error in nfq_create_queue() : " << _queueNumber << " : err = " << nfq_errno);
		throw StringException("Error in nfq_create_queue()");
	}

	// Turn on packet copy mode
	if (nfq_set_mode(_nfQueue, NFQNL_COPY_PACKET, Config::instance()->nfBufSize()) < 0) 
	{
		SLOG_ERROR( << "Could not set packet copy mode" );
		throw StringException("Error in nfq_set_mode()");
	}


	if (nfq_set_queue_maxlen(_nfQueue, Config::instance()->nfQueueSize()) < 0)
	{
		SLOG_ERROR( << "Could not resize nfQueue" );
		throw StringException("Error in nfq_set_queue_maxlen()");
	}

	SLOG_INFO(<< "Queue setup for [" << _queueNumber << "]");
}

PacketHandler::~PacketHandler()
{
	if(_nfQueue)
	{
		nfq_destroy_queue(_nfQueue);
	}

	
	if(_netlinkHandle)
	{
		nfq_close(_nfqHandle);
		SLOG_INFO(<< "Queue close for [" << _queueNumber << "]");
	}
}


void PacketHandler::run()
{
	SLOG_INFO( << "Entering event loop for PacketHandler : Queue [" << _queueNumber << "]")

	if(!_nfqHandle || !_nfQueue )
	{
		SLOG_ERROR( << "Not setup correctly [" << _queueNumber << "]");
		throw StringException("PacketHandler not constructed correctly");
	}

	int fd; 
	int res;
	
	_netlinkHandle = nfq_nfnlh(_nfqHandle);

	SLOG_INFO(<< "netlinkHandle : " << _netlinkHandle);
	nfnl_rcvbufsiz(_netlinkHandle, Config::instance()->nfQueueSize() * Config::instance()->nfBufSize());

	fd = nfnl_fd(_netlinkHandle);

	SLOG_INFO(<< "fd [" << fd << "] on queue [" << _queueNumber << "]");

	struct timeval timeout;
	// TODO maybe make configurable
	timeout.tv_sec = 1; 
	timeout.tv_usec = 0; 

	// Set a timeout on the socket so we can shutdown the thread
	res = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

	SLOG_INFO(<< "result of setting SO_RCVTIMEO [" << res << "]")

	while (!_shutdown) 
	{
		// TODO mempory pool to not do a malloc every packet
		_buffer = new u_char[Config::instance()->nfBufSize()];
		if(_buffer == NULL)
		{
			SLOG_ERROR(<< "Unable to allocate buffer for recv");
			continue;
		}

		res = recv(fd, _buffer, Config::instance()->nfBufSize(), 0);

		if(res >= 0)
		{
			nfq_handle_packet(_nfqHandle, (char*)_buffer, res);
		}
		else
		{
			delete _buffer;
			_buffer = NULL;
			if(errno != EAGAIN )
			{
				SLOG_ERROR(<< "errno [" << errno << "][" << strerror(errno) << "] res is [" << res << "]. Shutting down");
				_shutdown = true;
			}
		}
	}

	// Bind this handler to process IP packets...
	// We only care about IPv4 in initial version
	if (nfq_unbind_pf(_nfqHandle, AF_INET) < 0) 
	{
		SLOG_ERROR( << "Error in nfq_unbind_pf()" );
	}

	SLOG_INFO( << "finished with queue [" << _queueNumber << "] after [" 
				<< _numPackets << "] packets");
}

int PacketHandler::setVerdictLocal(uint32_t id, uint32_t verdict)
{
	if(verdict > NF_MAX_VERDICT || verdict < NF_DROP)
	{
		SLOG_ERROR( << "Invalid verdict [" << verdict << "]");
		return -1;
	}

	std::unique_lock<std::mutex> statsGuard(_statsMutex);
	_verdictStats[verdict]++;
	statsGuard.unlock();

	int ret = nfq_set_verdict(_nfQueue, id, verdict, 0, NULL);

	if(_buffer)
	{
		delete _buffer;
		_buffer = NULL;
	}
	return ret;
}

int PacketHandler::setVerdict(uint32_t id, uint32_t verdict)
{
	return setVerdictLocal(id, verdict);
}

std::vector<uint64_t> PacketHandler::verdictStats()
{
	std::lock_guard<std::mutex> guard(_statsMutex);
	return _verdictStats;
}

bool PacketHandler::handlePacket(struct nfq_q_handle* nfQueue, 
				struct nfgenmsg *msg,
            	struct nfq_data *pkt)
{
	_numPackets++;

	uint32_t id = 0;

	nfqnl_msg_packet_hdr *header = nfq_get_msg_packet_hdr(pkt);
	if (header) 
	{
		id = ntohl(header->packet_id);
	}

	timeval timestamp;
	if (nfq_get_timestamp(pkt, &timestamp) != 0) 
	{
		// We can't guarentee a timestamp messages on the OUTPUT queue.
		// For now just pop the current time into the timeval
		// Should be ok because we onlu use time for UDP connection tracking ??
		gettimeofday(&timestamp, NULL);
	}

	u_char *pktData;
	int pktLen = nfq_get_payload(pkt, &pktData);

	//SLOG_INFO( << "Packet len is " << pktLen );
	//if(pktLen > 0)
	//{
	//		LOG_HEXDUMP("data :", pktData, pktLen);
	//}

	struct ip_vhl {
		unsigned int ip_hl:4; // only in IPv4
		unsigned int ip_v:4;
    };	

    // IP Header
    const u_char* ip_hdr_start = pktData;

    ip_vhl* vhl = (struct ip_vhl*)(ip_hdr_start);

    if(vhl->ip_v == IPv4)
    {
		struct iphdr* ip_hdr = (struct iphdr*)(ip_hdr_start);
		const u_char* transport_hdr_start = ip_hdr_start + (vhl->ip_hl * 4);

		// extract the source and the destination ip-adrress
		in_addr* srcIp = (in_addr*) &ip_hdr->saddr;
		in_addr* dstIp = (in_addr*) &ip_hdr->daddr;

		char src[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET, srcIp, src, INET6_ADDRSTRLEN);

		char dst[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET, dstIp, dst, INET6_ADDRSTRLEN);

		// Transport Layer Header
		switch(ip_hdr->protocol)
		{
			case IPPROTO_ICMP:
			{
				SLOG_INFO( << "IPPROTO_ICMP");
				return setVerdictLocal(id, NF_ACCEPT);
			}
			case IPPROTO_TCP:
			{
				SLOG_INFO( << "IPPROTO_TCP : pktid=[" << id << "]");

				struct tcphdr* tcph = (struct tcphdr*)(transport_hdr_start);

				const u_char* data_start = transport_hdr_start  + tcph->doff * 4;
				
				// We don't care about the verdict so set it now.
				return setVerdict(id, NF_ACCEPT);
			}
			case IPPROTO_UDP:
			{
				SLOG_INFO( << "IPPROTO_UDP : pktid=[" << id << "]");

				const u_char* data_start = transport_hdr_start + sizeof(udphdr);

				return setVerdict(id, NF_ACCEPT);
			}
			case IPPROTO_SCTP:
			{
				SLOG_INFO( << "IPPROTO_SCTP");
				return setVerdictLocal(id, NF_ACCEPT);
			}
			default:
			{
				SLOG_INFO( << "UNKNOWN IPPROTO : " << ip_hdr->protocol);
				return setVerdictLocal(id, NF_ACCEPT);
			}

		}
	}
	else if(vhl->ip_v == IPv6)
	{
		SLOG_ERROR(<< "IP Version 6 not supported yet")
		return setVerdictLocal(id, NF_ACCEPT);
	}
	else
	{
		SLOG_ERROR(<< "Invalid IP Version : " << vhl->ip_v)
		return setVerdictLocal(id, NF_ACCEPT);
	}

    return setVerdictLocal(id, NF_ACCEPT);
}

void PacketHandler::shutdown()
{
	_shutdown = true;
	SLOG_INFO(<< "Shutdown called for queue [" << _queueNumber << "]");
}