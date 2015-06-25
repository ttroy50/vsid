/**
 * Pcap Reader
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_NETFILTER_PACKET_HANDLER_H__
#define __VSID_NETFILTER_PACKET_HANDLER_H__

#include <string>

extern "C" {
  #include <libnetfilter_queue/libnetfilter_queue.h>
}

namespace VsidNetfilter
{

class StringException : public std::exception
{
public:
	StringException(std::string what) { _what = what; }
	const char* what() const noexcept { return _what.c_str(); }
private:
	std::string _what;
};

class PacketHandler
{
public:
	PacketHandler(int queueNumber);
	~PacketHandler();

	void run();

	bool handlePacket(struct nfq_q_handle* nfQueue, 
				struct nfgenmsg *msg,
            	struct nfq_data *pkt);

	void shutdown();

	int setVerdict(int id, int verdict);

private:
	int _queueNumber;
	bool _shutdown;
	int _numPackets;

	struct nfq_handle* _nfqHandle;
	struct nfq_q_handle* _nfQueue;
	struct nfnl_handle* _netlinkHandle;
};

}

#endif