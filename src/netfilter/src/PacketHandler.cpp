#include <netinet/in.h>

extern "C" {
  #include <linux/netfilter.h>  /* Defines verdicts (NF_ACCEPT, etc) */
  #include <libnetfilter_queue/libnetfilter_queue.h>
}

#include <iostream>
#include <iomanip>

#include <time.h>


#include "PacketHandler.h"

#include "Logger.h"

using namespace std;
using namespace VsidNetfilter;

/* Definition of callback function */
static int cb(struct nfq_q_handle *myQueue, struct nfgenmsg *msg,
              struct nfq_data *pkt, void *data)
{
    int verdict;
    SLOG_INFO( << "Received packet" );

	uint32_t id = 0;
	nfqnl_msg_packet_hdr *header = nfq_get_msg_packet_hdr(pkt);

	if (header) 
	{
		id = ntohl(header->packet_id);
		SLOG_INFO (<< "id : " << id << " ; hw_protocol : " << ntohs(header->hw_protocol) << "; hook " << ('0'+header->hook)
		<< " ; ");
	}

	// The HW address is only fetchable at certain hook points
	nfqnl_msg_packet_hw *macAddr = nfq_get_packet_hw(pkt);
	if (macAddr) 
	{
		SLOG_INFO( << "mac len " << ntohs(macAddr->hw_addrlen) );
	} 
	else 
	{
		SLOG_INFO( << "no MAC addr" );
	}

	timeval tv;
	if (nfq_get_timestamp(pkt, &tv) == 0) 
	{
		SLOG_INFO( << "; tstamp " << tv.tv_sec << "." << tv.tv_usec );
	} 
	else 
	{
		// We can't guarentee a timestamp messages on the OUTPUT queue.
		// For now just pop the current time into the timeval
		// Should be ok because we onlu use time for UDP connection tracking ??
		gettimeofday(&tv, NULL);

		SLOG_INFO( << "; no tstamp in msg. Setting to :" << tv.tv_sec << "." << tv.tv_usec);
	}

	SLOG_INFO( << "; mark " << nfq_get_nfmark(pkt) );

	// Note that you can also get the physical devices
	SLOG_INFO( << "; indev " << nfq_get_indev(pkt) );
	SLOG_INFO( << "; outdev " << nfq_get_outdev(pkt) );

	// Print the payload; in copy meta mode, only headers will be included;
	// in copy packet mode, whole packet will be returned.
	
	u_char *pktData;
	int len = nfq_get_payload(pkt, &pktData);
	SLOG_INFO( << "Packet len is " << len );
	if(len > 0)
	{
		LOG_HEXDUMP("data :", pktData, len);
		SLOG_INFO( << pktData)
	}

    return nfq_set_verdict(myQueue, id, NF_ACCEPT, 0, NULL); /* Verdict packet */
}


void PacketHandler::run()
{
	struct nfq_handle *nfqHandle;

	struct nfq_q_handle *myQueue;
	struct nfnl_handle *netlinkHandle;

	int fd, res;
	char buf[4096];

	// Get a queue connection handle from the module
	if (!(nfqHandle = nfq_open())) 
	{
		SLOG_ERROR( << "Error in nfq_open()" );
		return;
	}

	// Unbind the handler from processing any IP packets
	// Not totally sure why this is done, or if it's necessary...
	if (nfq_unbind_pf(nfqHandle, AF_INET) < 0) 
	{
		cerr << "Error in nfq_unbind_pf()" << endl;
		exit(1);
	}

	// Bind this handler to process IP packets...
	if (nfq_bind_pf(nfqHandle, AF_INET) < 0) {
		cerr << "Error in nfq_bind_pf()" << endl;
		exit(1);
	}

	// Install a callback on queue 0
	if (!(myQueue = nfq_create_queue(nfqHandle,  0, &cb, NULL))) {
		cerr << "Error in nfq_create_queue()" << endl;
		exit(1);
	}

	// Turn on packet copy mode
	if (nfq_set_mode(myQueue, NFQNL_COPY_PACKET, 0xffff) < 0) {
		cerr << "Could not set packet copy mode" << endl;
		exit(1);
	}

	netlinkHandle = nfq_nfnlh(nfqHandle);
	fd = nfnl_fd(netlinkHandle);

	while ((res = recv(fd, buf, sizeof(buf), 0)) && res >= 0) {
		// I am not totally sure why a callback mechanism is used
		// rather than just handling it directly here, but that
		// seems to be the convention...
		nfq_handle_packet(nfqHandle, buf, res);
		// end while receiving traffic
	}

	nfq_destroy_queue(myQueue);

	nfq_close(nfqHandle);

	return;

}