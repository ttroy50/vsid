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

#include "IPv4.h"
#include "IPv4Tuple.h"
#include "Logger.h"

namespace Vsid
{
	class AttributeMeter;
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
		FINISHED
	};

	Flow(IPv4Packet* packet);

	Flow(IPv4Tuple tuple);

	/** 
	 * Special construct to create empty flow for lookup of the unorded_set / map
	 * If set will return the hash in the hash function and always true 
	 * in equality function
	 */
	Flow(uint32_t hash);

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
	uint32_t flowHash();


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

	uint32_t pktCount() const { return _pktCount; } 

	const struct timeval& startTimestamp() const { return _startTimestamp; }

	const struct timeval& lastPacketTimestamp() const { return _lastPacketTimestamp; }
	Direction lastPacketDirection() const { return _lastPacketDirection; }

	u_char* firstOrigToDestData() { return _firstOrigToDestData; }
	size_t firstOrigToDestDataSize() { return _firstOrigToDestDataSize; }

	bool isFirstPacket() { return _isFirstPacket; }

	Direction currentPacketDirection() const { return _currentPacketDirection; }

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

	State _flowState;

	std::vector<std::shared_ptr<Vsid::AttributeMeter> > _attributeMeters;

	

	


};

	inline bool operator==(const Flow& lhs, const Flow& rhs)
	{
		if(lhs._hash > 0)
			return true;

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

		os << "count : " << flow.pktCount() 
			<< " state : " << static_cast<std::underlying_type<Flow::State>::type>(flow.flowState())
			<< " | transport : " << (uint32_t)flow.fiveTuple().transport 
	    	<< " | src : " << src << ":" << flow.fiveTuple().src_port 
	    	<< " | dst : " << dst << ":" << flow.fiveTuple().dst_port;
	}

} // end namespace

#endif // END HEADER GUARD