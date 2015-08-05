#include "IPv4.h"
#include "Logger.h"
#include "Hasher.h"

using namespace std;
using namespace VsidCommon;


IPv4Packet::~IPv4Packet()
{
	// make sure a packet has a verdict set
	if( _verdictSetter )
	{
		_verdictSetter->setDrop(_pktId);
		_verdictSetter = NULL;
	}


	if(_buffer)
	{
		delete[] _buffer;
	}
}

uint32_t IPv4Packet::flowHash()
{
	Ipv4FlowHasher hasher;
	return hasher(this);
}

bool IPv4Packet::sameFlow(IPv4Packet& rhs)
{
	SLOG_INFO(<< "checking flow");
	
	//SLOG_INFO( << "lhs : " << *this);
	//SLOG_INFO( << "rhs : " << rhs);

	if(protocol() != rhs.protocol())
	{
		SLOG_INFO(<< "Not the same flow proto")
		return false;
	}

	if(srcIp() == rhs.srcIp() 
			&& dstIp() == rhs.dstIp()
			&& srcPort() == rhs.srcPort()
			&& dstPort() == rhs.dstPort())
	{
		// same flow and direction
		 SLOG_INFO(<< "same flow and direction")
		return true;
	}
	else if( srcIp() == rhs.dstIp() 
			&& dstIp() == rhs.srcIp()
			&& srcPort() == rhs.dstPort()
			&& dstPort() == rhs.srcPort())
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


void IPv4Packet::setVerdict()
{
	// Default to accept for now.
	acceptPkt();
}

void IPv4Packet::acceptPkt()
{
	if ( _verdictSetter )
	{
		_verdictSetter->setAccept(_pktId);
		_verdictSetter = NULL;
	}
}

void IPv4Packet::dropPkt()
{
	if ( _verdictSetter )
	{
		_verdictSetter->setDrop(_pktId);
		_verdictSetter = NULL;
	}
}