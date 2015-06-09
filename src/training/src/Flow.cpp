#include "Flow.h"
#include "Logger.h"
#include "Hasher.h"
#include "IPv4.h"
using namespace std;
using namespace VSID_TRAINING;


Flow::Flow(IPv4* packet)
{
	_firstPacketTuple.src_ip = packet->srcIp();
	_firstPacketTuple.src_port = packet->srcPort();
	_firstPacketTuple.dst_ip = packet->dstIp();
	_firstPacketTuple.dst_port = packet->dstPort();
	_firstPacketTuple.transport = packet->protocol();
}


Flow::Flow(IPv4Tuple tuple) :
	_firstPacketTuple(tuple)
{

}

void Flow::addPacket(IPv4* packet)
{

}

uint32_t Flow::flowHash()
{
	Ipv4FlowHasher hasher;
	return hasher(&_firstPacketTuple);
}