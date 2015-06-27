#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

extern "C" {
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

#include "Config.h"

using namespace std;
using namespace VsidNetfilter;
using namespace VsidCommon;

/* Definition of callback function */
static int nfqPacketHandlerCb(struct nfq_q_handle* nfQueue, 
				struct nfgenmsg *msg,
            	struct nfq_data *pkt, 
            	void *data)
{
    int verdict = NF_ACCEPT;

    uint32_t id = 0;
	nfqnl_msg_packet_hdr *header = nfq_get_msg_packet_hdr(pkt);

	if (header) 
	{
		id = ntohl(header->packet_id);
	}

    if(data == NULL)
    {
    	SLOG_ERROR(<< "packet handler is NULL");
    	return nfq_set_verdict(nfQueue, id, verdict, 0, NULL); /* Verdict packet */
    }

    SLOG_ERROR(<< "handler for nfQUeue " << nfQueue);

    PacketHandler* handler = static_cast<PacketHandler*>(data);
    if ( !handler->handlePacket(nfQueue, msg, pkt) )
    {
    	SLOG_INFO( << "Verdict not set in handlePacket")
    	return nfq_set_verdict(nfQueue, id, verdict, 0, NULL); /* Verdict packet */
    }
}

PacketHandler::PacketHandler(int queueNumber) :
	_shutdown(false),
	_numPackets(0)
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

	// Install a callback on queue 0
	if (!(_nfQueue = nfq_create_queue(_nfqHandle, _queueNumber, &nfqPacketHandlerCb, static_cast<void*>(this)))) 
	{
		SLOG_ERROR( << "Error in nfq_create_queue()" );
		throw StringException("Error in nfq_create_queue()");
	}

	// Turn on packet copy mode
	if (nfq_set_mode(_nfQueue, NFQNL_COPY_PACKET, Config::instance()->nfBufSize()) < 0) 
	{
		SLOG_ERROR( << "Could not set packet copy mode" );
		throw StringException("Error in nfq_set_mode()");
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
	char buf[4096];
	
	_netlinkHandle = nfq_nfnlh(_nfqHandle);

	SLOG_INFO(<< "netlinkHandle : " << _netlinkHandle);
	nfnl_rcvbufsiz(_netlinkHandle, Config::instance()->nfQueueSize() * Config::instance()->nfBufSize());

	fd = nfnl_fd(_netlinkHandle);

	SLOG_INFO(<< "fd [" << fd << "] on queue [" << _queueNumber << "]");

	struct timeval timeout;
	// TODO maybe make configurable
	timeout.tv_sec = 5; 
	timeout.tv_usec = 0; 

	// Set a timeout on the socket so we can shutdown the thread
	res = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

	SLOG_INFO(<< "result of setting SO_RCVTIMEO [" << res << "]")

	while (!_shutdown) 
	{
		res = recv(fd, buf, sizeof(buf), 0);

		if(res >= 0)
		{
			nfq_handle_packet(_nfqHandle, buf, res);
		}
		else
		{
			if(errno != EAGAIN )
			{
				SLOG_ERROR(<< "errno [" << errno << "][" << strerror(errno) << "] res is [" << res << "]. Shutting down");
				_shutdown = true;
			}
			else
			{
				SLOG_INFO(<< "EGAAIN on " << _queueNumber);
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


int PacketHandler::setVerdict(int id, int verdict)
{
	SLOG_INFO(<< "set verdict : " << id);
	return nfq_set_verdict(_nfQueue, id, verdict, 0, NULL);
}

bool PacketHandler::handlePacket(struct nfq_q_handle* nfQueue, 
				struct nfgenmsg *msg,
            	struct nfq_data *pkt)
{
	_numPackets++;
    SLOG_INFO( << "Received packet : " << _numPackets );

	uint32_t id = 0;

	nfqnl_msg_packet_hdr *header = nfq_get_msg_packet_hdr(pkt);
	if (header) 
	{
		id = ntohl(header->packet_id);
		SLOG_INFO(<< "packet id : " << id << " | on queue : " << _queueNumber);
	}

	timeval timestamp;
	if (nfq_get_timestamp(pkt, &timestamp) == 0) 
	{
		SLOG_INFO( << "; tstamp " << timestamp.tv_sec << "." << timestamp.tv_usec );
	} 
	else 
	{
		// We can't guarentee a timestamp messages on the OUTPUT queue.
		// For now just pop the current time into the timeval
		// Should be ok because we onlu use time for UDP connection tracking ??
		gettimeofday(&timestamp, NULL);

		SLOG_INFO( << "no tstamp in msg. Setting to :" << timestamp.tv_sec << "." << timestamp.tv_usec);
	}

	SLOG_INFO( << "; mark " << nfq_get_nfmark(pkt) );

	u_char *pktData;
	int pktLen = nfq_get_payload(pkt, &pktData);

	SLOG_INFO( << "Packet len is " << pktLen );
	/*if(pktLen > 0)
	{
		LOG_HEXDUMP("data :", pktData, pktLen);
	}*/

	struct ip_vhl {
		unsigned int ip_hl:4; // only in IPv4
		unsigned int ip_v:4;
    };	

    // IP Header
    const u_char* ip_hdr_start = pktData;

    ip_vhl* vhl = (struct ip_vhl*)(ip_hdr_start);
    SLOG_INFO(<< "IP - HLen : " << vhl->ip_hl * 4 << " - Ver : " << vhl->ip_v);

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

		SLOG_INFO( << "src ip : " << src );
		SLOG_INFO( << "dst ip : " << dst );

		// Transport Layer Header
		switch(ip_hdr->protocol)
		{
			case IPPROTO_ICMP:
			{
				SLOG_INFO( << "IPPROTO_ICMP");
				return setVerdict(id, NF_ACCEPT);
			}
			case IPPROTO_TCP:
			{
				SLOG_INFO( << "IPPROTO_TCP");

				const u_char* data_start = transport_hdr_start + sizeof(tcphdr);
				
				TcpIPv4 tcp(pktData, pktLen, ip_hdr_start, 
							transport_hdr_start, data_start, 
							timestamp);
				
				SLOG_INFO( << "src port : " << tcp.srcPort());
				SLOG_INFO( << "dst port : " << tcp.dstPort());

				_flowManager.addPacket(&tcp);

				return setVerdict(id, NF_ACCEPT);
			}
			case IPPROTO_UDP:
			{
				SLOG_INFO( << "IPPROTO_UDP");

				const u_char* data_start = transport_hdr_start + sizeof(udphdr);

				UdpIPv4 udp(pktData, pktLen, ip_hdr_start, 
							transport_hdr_start, data_start, 
							timestamp);


				SLOG_INFO( << "src port : " << udp.srcPort());
				SLOG_INFO( << "dst port : " << udp.dstPort());

				_flowManager.addPacket(&udp);

				return setVerdict(id, NF_ACCEPT);
			}
			case IPPROTO_SCTP:
			{
				SLOG_INFO( << "IPPROTO_SCTP");
				return setVerdict(id, NF_ACCEPT);
			}
			default:
			{
				SLOG_INFO( << "UNKNOWN IPPROTO : " << ip_hdr->protocol);
				return setVerdict(id, NF_ACCEPT);
			}

		}
	}
	else if(vhl->ip_v == IPv6)
	{
		SLOG_ERROR(<< "IP Version 6 not supported yet")
		return setVerdict(id, NF_ACCEPT);
	}
	else
	{
		SLOG_ERROR(<< "Invalid IP Version : " << vhl->ip_v)
		return setVerdict(id, NF_ACCEPT);
	}

    return setVerdict(id, NF_ACCEPT); /* Verdict packet */
}

void PacketHandler::shutdown()
{
	_shutdown = true;
	SLOG_INFO(<< "Shutdown called for queue [" << _queueNumber << "]");
}