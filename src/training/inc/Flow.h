/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_FLOW_H__
#define __VSID_FLOW_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include "IPv4.h"
#include "IPv4Tuple.h"
#include "Logger.h"

namespace VSID_TRAINING
{

class Flow
{
public:
	enum Direction 
	{
		ORIG_TO_DEST,
		DEST_TO_ORIG,
		UNKNOWN
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
		SLOG_INFO(<< "checking flow");

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
			return UNKNOWN;

		if(_firstPacketTuple.srcIp() == rhs->srcIp() 
				&& _firstPacketTuple.dstIp() == rhs->dstIp()
				&& _firstPacketTuple.srcPort() == rhs->srcPort()
				&& _firstPacketTuple.dstPort() == rhs->dstPort())
		{
			// same flow and direction
			
			return ORIG_TO_DEST;
		}
		else if( _firstPacketTuple.srcIp() == rhs->dstIp() 
				&& _firstPacketTuple.dstIp() == rhs->srcIp()
				&& _firstPacketTuple.srcPort() == rhs->dstPort()
				&& _firstPacketTuple.dstPort() == rhs->srcPort())
		{
			// same flow different direction
			return DEST_TO_ORIG;
		}
		else
		{
			return UNKNOWN;
		}
	}

	uint32_t pktCount() const { return _pktCount; } 

private:
	IPv4Tuple _firstPacketTuple;
	uint32_t _hash;
	uint32_t _pktCount;
};

	inline bool operator==(const Flow& lhs, const Flow& rhs)
	{
		SLOG_INFO(<< "checking flow ==");
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
			<< " | transport : " << flow.fiveTuple().transport 
	    	<< " | src : " << src << ":" << flow.fiveTuple().src_port 
	    	<< " | dst : " << dst << ":" << flow.fiveTuple().dst_port;
	}

} // end namespace

#endif // END HEADER GUARD