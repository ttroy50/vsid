#include "Flow.h"
#include "Logger.h"
#include "Hasher.h"
#include "IPv4.h"
using namespace std;
using namespace VSID_TRAINING;


Flow::Flow(IPv4Packet* packet) :
	_hash(0),
	_pktCount(0)
{
	_firstPacketTuple.src_ip = packet->srcIp();
	_firstPacketTuple.src_port = packet->srcPort();
	_firstPacketTuple.dst_ip = packet->dstIp();
	_firstPacketTuple.dst_port = packet->dstPort();
	_firstPacketTuple.transport = packet->protocol();
}


Flow::Flow(IPv4Tuple tuple) :
	_firstPacketTuple(tuple),
	_hash(0),
	_pktCount(0)
{

}

Flow::Flow(uint32_t hash) :
	_hash(hash),
	_pktCount(0)
{}

void Flow::addPacket(IPv4Packet* packet)
{
	SLOG_INFO(<< "Packet added to flow");
	_pktCount++;
}

uint32_t Flow::flowHash()
{
	if(_hash > 0)
		return _hash;

	Ipv4FlowHasher hasher;
	return hasher(&_firstPacketTuple);
}