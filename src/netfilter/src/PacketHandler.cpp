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

    PacketHandler* handler = static_cast<PacketHandler*>(data);
    if ( !handler->handlePacket(nfQueue, msg, pkt) )
    {
    	SLOG_INFO( << "Verdict not set in handlePacket")
    	return nfq_set_verdict(nfQueue, id, verdict, 0, NULL); /* Verdict packet */
    }
}

PacketHandler::PacketHandler(int queueNumber) :
	_queueNumber(queueNumber),
	_shutdown(false),
	_numPackets(0)
{
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
	if (nfq_set_mode(_nfQueue, NFQNL_COPY_PACKET, 0xffff) < 0) 
	{
		SLOG_ERROR( << "Could not set packet copy mode" );
		throw StringException("Error in nfq_set_mode()");
	}
}

PacketHandler::~PacketHandler()
{
	if(_nfQueue)
		nfq_destroy_queue(_nfQueue);

	if(_netlinkHandle)
		nfq_close(_nfqHandle);
}


void PacketHandler::run()
{
	SLOG_INFO( << "Entering event loop for PacketHandler : Queue [" << _queueNumber << "]")

	if(!_nfqHandle || !_nfQueue )
	{
		SLOG_ERROR( << "Not setup correctly")
		throw StringException("PacketHandler not constructed correctly");
	}

	int fd; 
	int res;
	char buf[4096];
	
	_netlinkHandle = nfq_nfnlh(_nfqHandle);
	fd = nfnl_fd(_netlinkHandle);

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
				SLOG_ERROR(<< "errno [" << errno << "] res is [" << res << "]. Shutting down");
				_shutdown = true;
			}
		}
	}
}


int PacketHandler::setVerdict(int id, int verdict)
{
	return nfq_set_verdict(_nfQueue, id, verdict, 0, NULL);
}

bool PacketHandler::handlePacket(struct nfq_q_handle* nfQueue, 
				struct nfgenmsg *msg,
            	struct nfq_data *pkt)
{
	_numPackets++;
    SLOG_INFO( << "Received packet : " << _numPackets );

    if(nfQueue != _nfQueue)
    {
    	SLOG_ERROR(<< "received nfQueue different from stored one")
    	return false;
    }

	uint32_t id = 0;
	nfqnl_msg_packet_hdr *header = nfq_get_msg_packet_hdr(pkt);

	if (header) 
	{
		id = ntohl(header->packet_id);
		SLOG_INFO (<< "id : " << id 
					<< " ; hw_protocol : " << ntohs(header->hw_protocol) 
					<< "; hook " << ('0'+ header->hook) );
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
	int len = nfq_get_payload(pkt, &pktData);
	SLOG_INFO( << "Packet len is " << len );
	if(len > 0)
	{
		LOG_HEXDUMP("data :", pktData, len);
		SLOG_INFO( << pktData)
	}

    return setVerdict(id, NF_ACCEPT); /* Verdict packet */
}

void PacketHandler::shutdown()
{
	_shutdown = true;
	SLOG_INFO(<< "Shutdown called for queue [" << _queueNumber << "]");
}