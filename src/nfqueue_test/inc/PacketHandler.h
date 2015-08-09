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
#include "FlowClassificationLogger.h"
#include "PacketCallbacks.h"

namespace Vsid
{
	class ProtocolModelDb;
}

namespace VsidNetfilter
{

class PacketHandler
{
public:
	PacketHandler(int queueNumber );
	~PacketHandler();

	void run();

	bool handlePacket(struct nfq_q_handle* nfQueue, 
				struct nfgenmsg *msg,
            	struct nfq_data *pkt);

	void shutdown();

	int setVerdict(uint32_t id, uint32_t verdict);

	/*
	 * Should only be called from same thread, to release the received buffer
	 */
	int setVerdictLocal(uint32_t id, uint32_t verdict);

	int queueNumber() { return _queueNumber; }
	uint64_t numPackets() { return _numPackets; }
	std::vector<uint64_t> verdictStats();

private:
	int _queueNumber;
	std::atomic<bool> _shutdown;

	std::atomic<uint64_t> _numPackets;
	std::vector<uint64_t> _verdictStats;

	struct nfq_handle* _nfqHandle;
	struct nfq_q_handle* _nfQueue;
	struct nfnl_handle* _netlinkHandle;

	size_t _queueSize;
	size_t _bufSize;

	/**
	 * Buffer pointer used for every loop of the recv
	 */
	u_char* _buffer;
    std::mutex _statsMutex;
};

}

#endif