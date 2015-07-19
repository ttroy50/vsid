/**
 * Pcap Reader
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_NETFILTER_PACKET_HANDLER_H__
#define __VSID_NETFILTER_PACKET_HANDLER_H__

#include <netinet/in.h>

extern "C" {
	#include <linux/netfilter.h>
	#include <libnetfilter_queue/libnetfilter_queue.h>
}

#include <string>
#include <atomic>
#include <mutex>

#include "FlowManager.h"

namespace Vsid
{
	class ProtocolModelDb;
}

namespace VsidNetfilter
{

class PacketHandler
{
public:
	PacketHandler(int queueNumber, Vsid::ProtocolModelDb* database);
	~PacketHandler();

	void run();

	bool handlePacket(struct nfq_q_handle* nfQueue, 
				struct nfgenmsg *msg,
            	struct nfq_data *pkt);

	void shutdown();

	int setVerdict(int id, int verdict);

	int queueNumber() { return _queueNumber; }
	uint64_t numPackets() { return _numPackets; }
	std::vector<uint64_t> verdictStats();

private:
	int _queueNumber;
	std::atomic<bool> _shutdown;

	std::atomic<uint64_t> _numPackets;
	std::vector<uint64_t> _verdictStats;
	std::mutex _statsMutex;

	struct nfq_handle* _nfqHandle;
	struct nfq_q_handle* _nfQueue;
	struct nfnl_handle* _netlinkHandle;

	size_t _queueSize;
	size_t _bufSize;

	VsidCommon::FlowManager _flowManager;
	Vsid::ProtocolModelDb* _prococolModelDb;
};

}

#endif