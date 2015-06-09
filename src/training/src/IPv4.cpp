#include "IPv4.h"
#include "Logger.h"
#include "Hasher.h"

using namespace std;
using namespace VSID_TRAINING;


uint32_t IPv4::flowHash()
{
	Ipv4FlowHasher hasher;
	return hasher(this);
}

bool IPv4::sameFlow(IPv4& rhs)
{
	if(protocol() != rhs.protocol())
		return false;

	if(srcIp() == rhs.srcIp() 
			&& dstIp() == rhs.dstIp()
			&& srcPort() == rhs.srcPort()
			&& dstPort() == rhs.dstPort())
	{
		// same flow and direction
		return true;
	}
	else if( srcIp() == rhs.dstIp() 
			&& dstIp() == rhs.srcIp()
			&& srcPort() == rhs.dstPort()
			&& dstPort() == rhs.srcPort())
	{
		// same flow different direction
		return true;
	}
	else
	{
		return false;
	}
}