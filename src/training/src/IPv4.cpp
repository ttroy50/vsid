#include "IPv4.h"
#include "Logger.h"
#include "Hasher.h"

using namespace std;
using namespace VSID_TRAINING;


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