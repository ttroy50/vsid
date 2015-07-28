/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_FLOW_H__
#define __VSID_FLOW_H__

#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <atomic>

#include "IPv4.h"
#include "IPv4Tuple.h"
#include "Logger.h"

namespace Vsid
{
	class AttributeMeter;
	class ProtocolModelDb;
}

#define PACKET_MAX_BUFFER_SIZE 2048

namespace VsidCommon
{

class Flow
{
public:
	enum class Direction 
	{
		ORIG_TO_DEST,
		DEST_TO_ORIG,
		UNKNOWN
	};

	enum class State
	{
		NEW,
		ESTABLISHING, // For TCP really means in establishment
		ESTABLISHED,
		FINISHING,
		FINISHED,
		FINISHED_NOTIFIED
	};

	Flow(IPv4Packet* packet, Vsid::ProtocolModelDb* database);

	void addPacket(IPv4Packet* packet);

	/**
	 * Get the fiveTuple representing this flow.
	 * This is taken from the first packet on the flow
	 * @return
	 */
	const IPv4Tuple& fiveTuple() const { return _firstPacketTuple; }

	/**
	 * Get a hash to represent the flow
	 * @return [description]
	 */
	uint32_t flowHash() const { return _hash; }


	friend bool operator==(const Flow& lhs, const Flow& rhs);
	friend bool operator!=(const Flow& lhs, const Flow& rhs);

	/**
	 * Check if the packet is part fo the same flow
	 * @param  packet
	 * @return
	 */
	template <typename T>
	bool sameFlow(const T* rhs) const
	{
		if(_firstPacketTuple.protocol() != rhs->protocol())
		{
			SLOG_INFO(<< "Not the same flow proto")
			return false;
		}

		if(_firstPacketTuple.srcIp() == rhs->srcIp() 
				&& _firstPacketTuple.dstIp() == rhs->dstIp()
				&& _firstPacketTuple.srcPort() == rhs->srcPort()
				&& _firstPacketTuple.dstPort() == rhs->dstPort())
		{
			// same flow and direction
			SLOG_INFO(<< "same flow and direction")
			return true;
		}
		else if( _firstPacketTuple.srcIp() == rhs->dstIp() 
				&& _firstPacketTuple.dstIp() == rhs->srcIp()
				&& _firstPacketTuple.srcPort() == rhs->dstPort()
				&& _firstPacketTuple.dstPort() == rhs->srcPort())
		{
			// same flow different direction
			SLOG_INFO(<< "same flow different direction")
			return true;
		}
		else
		{
			SLOG_INFO(<< "Not the same flow")
			return false;
		}
	}

	/**
	 * Check the direction of the packet in the flow
	 * UNKNOWN means it's not part of the flow
	 * @param  packet
	 * @return
	 */
	template <typename T>
	Direction packetDirection(const T* rhs) const
	{
		if(_firstPacketTuple.protocol() != rhs->protocol())
			return Direction::UNKNOWN;

		if(_firstPacketTuple.srcIp() == rhs->srcIp() 
				&& _firstPacketTuple.dstIp() == rhs->dstIp()
				&& _firstPacketTuple.srcPort() == rhs->srcPort()
				&& _firstPacketTuple.dstPort() == rhs->dstPort())
		{
			// same flow and direction
			
			return Direction::ORIG_TO_DEST;
		}
		else if( _firstPacketTuple.srcIp() == rhs->dstIp() 
				&& _firstPacketTuple.dstIp() == rhs->srcIp()
				&& _firstPacketTuple.srcPort() == rhs->dstPort()
				&& _firstPacketTuple.dstPort() == rhs->srcPort())
		{
			// same flow different direction
			return Direction::DEST_TO_ORIG;
		}
		else
		{
			return Direction::UNKNOWN;
		}
	}

	State flowState() const { return _flowState; }

	/**
	 * Packet count for what are considerd data packets.
	 * This doesn't consider TCP packets such as SYN, ACK (with no data)
	 * @return
	 */
	uint32_t pktCount() const { return _pktCount; } 

	/**
	 * Packet count of all packets on a flow
	 * @return
	 */
	uint32_t overallPktCount() const { return _overallPktCount; } 

	/**
	 * Start time of the flow
	 * @return
	 */
	const struct timeval& startTimestamp() const { return _startTimestamp; }

	/**
	 * Timestamp of the last packet in the flow
	 * @return
	 */
	const struct timeval& lastPacketTimestamp() const { return _lastPacketTimestamp; }

	/**
	 * Direction of the last packet in the flow
	 * @return
	 */
	Direction lastPacketDirection() const { return _lastPacketDirection; }

	/**
	 * Data portion of the first packet in the flow
	 * @return
	 */
	u_char* firstOrigToDestData() { return _firstOrigToDestData; }
	size_t firstOrigToDestDataSize() { return _firstOrigToDestDataSize; }

	bool isFirstPacket() { return _isFirstPacket; }

	/**
	 * Direction of the current packet in the flow
	 * @return
	 */
	Direction currentPacketDirection() const { return _currentPacketDirection; }

	const std::vector<std::shared_ptr<Vsid::AttributeMeter> >& attributeMeters() { return _attributeMeters; }
	
	bool flowClassified() const { return _flowClassified; }
	double classifiedDivergence() const { return _classifiedDivergence; }
	std::string classifiedProtocol() const { return _classifiedProtocol; }

	double bestMatchDivergence() const { return _bestMatchDivergence; }
	std::string bestMatchProtocol() const { return _bestMatchProtocol; }
	
	/**
	 * ID of the thread queue that handles this 
	 * @return
	 */
	int threadQueueId() const { return _threadQueueId; }
	void threadQueueId(int id) { _threadQueueId = id; }

	void incPktsInQueue() { _pktsInQueue++; }
	void decPktsInQueue() { _pktsInQueue--; }
	bool havePktsInQueue() { return _pktsInQueue > 0; }
	void setFinishedAndNotified() { _flowState =  Flow::State::FINISHED_NOTIFIED;}
	
private:

	IPv4Tuple _firstPacketTuple;
	struct timeval _startTimestamp;

	// for TCP this is first after SYN
	u_char _firstOrigToDestData[PACKET_MAX_BUFFER_SIZE];
	size_t _firstOrigToDestDataSize;
	
	bool _isFirstPacket;
	Direction _currentPacketDirection;

	struct timeval _lastPacketTimestamp;
	Direction _lastPacketDirection;

	uint32_t _hash;
	uint32_t _pktCount;
	uint32_t _overallPktCount;

	State _flowState;

	std::vector<std::shared_ptr<Vsid::AttributeMeter> > _attributeMeters;
	std::map<std::string, std::shared_ptr<Vsid::AttributeMeter> > _attributeMetersMap;
	
	// Have we classified the protocol
	bool _flowClassified;
	std::string _classifiedProtocol;
	double _classifiedDivergence;


	double _bestMatchDivergence;
	std::string _bestMatchProtocol;

	Vsid::ProtocolModelDb* _protocolModelDb;

	int _threadQueueId;
	std::atomic<uint64_t> _pktsInQueue;
};

	inline bool operator==(const Flow& lhs, const Flow& rhs)
	{
		return lhs.sameFlow(&rhs.fiveTuple());
	}

	inline bool operator!=(const Flow& lhs, const Flow& rhs)
	{
		return !(lhs == rhs);
	}
	
	inline MAKE_LOGGABLE(Flow, flow, os)
	{
		char src[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET, &flow.fiveTuple().src_ip, src, INET6_ADDRSTRLEN);

		char dst[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET, &flow.fiveTuple().dst_ip , dst, INET6_ADDRSTRLEN);

		os  << "{"
			<< "pkt_count: " << flow.pktCount() 
			<< ", overall_pkt_count: " << flow.overallPktCount() 
			<< ", state: " << static_cast<std::underlying_type<Flow::State>::type>(flow.flowState())
			<< ", hash: " << flow.flowHash()
			<< ", transport: " << (uint32_t)flow.fiveTuple().transport 
	    	<< ", src: '" << src << ":" << flow.fiveTuple().src_port << "'" 
	    	<< ", dst: '" << dst << ":" << flow.fiveTuple().dst_port << "'" 
	    	<< ", classified: " << (flow.flowClassified() ? "true" : "false")
	    	<< "}";
	}

} // end namespace

#endif // END HEADER GUARD