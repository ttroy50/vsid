#ifndef __VSID_FLOW_H__
#define __VSID_FLOW_H__

#include "IPv4.h"
#include "IPv4Tuple.h"

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

	Flow(IPv4* packet);

	Flow(IPv4Tuple tuple);

	void addPacket(IPv4* packet);

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
			return false;

		if(_firstPacketTuple.srcIp() == rhs->srcIp() 
				&& _firstPacketTuple.dstIp() == rhs->dstIp()
				&& _firstPacketTuple.srcPort() == rhs->srcPort()
				&& _firstPacketTuple.dstPort() == rhs->dstPort())
		{
			// same flow and direction
			return true;
		}
		else if( _firstPacketTuple.srcIp() == rhs->dstIp() 
				&& _firstPacketTuple.dstIp() == rhs->srcIp()
				&& _firstPacketTuple.srcPort() == rhs->dstPort()
				&& _firstPacketTuple.dstPort() == rhs->srcPort())
		{
			// same flow different direction
			return true;
		}
		else
		{
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

private:
	IPv4Tuple _firstPacketTuple;
};

inline bool operator==(const Flow& lhs, const Flow& rhs)
{
	return lhs.sameFlow(&rhs.fiveTuple());
}

inline bool operator!=(const Flow& lhs, const Flow& rhs)
{
	return !(lhs == rhs);
}

}

#endif