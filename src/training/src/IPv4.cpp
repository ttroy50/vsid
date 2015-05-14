#include "IPv4.h"
#include "murmur3.h"
#include "Logger.h"

using namespace std;
using namespace VSID_TRAINING;


	/**
	 * http://stackoverflow.com/questions/3215232/hash-function-for-src-dest-ip-port
	 * Potential hash functtion
	 * ((size_t)(key.src.s_addr) * 59) ^
((size_t)(key.dst.s_addr)) ^
((size_t)(key.sport) << 16) ^
((size_t)(key.dport)) ^
((size_t)(key.proto));

https://www.cs.auckland.ac.nz/~brian/flowhashRep.pdf
http://stackoverflow.com/questions/29258860/hash-function-for-5-tuple-ipv4-6?lq=1

32				16			32				16				8
srcip 		srcport 		dstip 		dstport 		protocol
1499754874		5060		1499754764		8888			12

1499754764		8888		1499754874		5060			12



String value 	89.100.113.122
Binary 	01011001 . 01100100 . 01110001 . 01111010
Integer 	1499754874


String value 	89.100.113.12
Binary 	01011001 . 01100100 . 01110001 . 00001100
Integer 	1499754764


https://searchcode.com/codesearch/view/9626801/
void __skb_get_rxhash(struct sk_buff *skb)

	 */
	

uint32_t IPv4::flowHash()
{
	uint32_t input[4];

	/*
	 * I think this should work. The idea is that no matter what direction the packet is on the flow, 
	 * it will generated the same hash
	 * 
	 * take flow starting
	 * ORIGINATOR -> RESPONDER		192.168.1.1:3333			192.168.1.5:5060  TCP
	 * RESPONDER -> ORIGINATOR		192.168.1.5:5060			192.168.1.1:3333  TCP
	 * This will generate 	192.168.1.1 192.168.1.5 33335060 TCP 	for both directions
	 *
	 * Copared to the flow for
	 * ORIGINATOR -> RESPONDER		192.168.1.5:3333		192.168.1.1:5060	TCP
	 * RESPONDER -> ORIGINATOR		192.168.1.1:5060		192.168.1.5:3333		TCP
	 * Which will generated  	192.168.1.1	192.168.1.5	50603333 TCP	
	 * 
	 *
	 * When the IP is the same we fallback to ports
	 * take flow starting
	 * ORIGINATOR -> RESPONDER		192.168.1.1:3333			192.168.1.1:5060  TCP
	 * RESPONDER -> ORIGINATOR		192.168.1.1:5060			192.168.1.1:3333  TCP
	 * This will generate 	192.168.1.1 192.168.1.1 33335060 TCP 	for both directions
	 *
	 *
	 * The above is the same as the connection in the other direction, however because these
	 * are on the same IP it should be same to assume they are different connections
	 * ORIGINATOR -> RESPONDER		192.168.1.1:5060			192.168.1.1:3333  TCP
	 * RESPONDER -> ORIGINATOR		192.168.1.1:3333			192.168.1.1:5050  TCP
	 * This will generate 	192.168.1.1 192.168.1.1 33335060 TCP 	for both directions
	 */

	input[2] = 0;

	if( srcIp()->s_addr < dstIp()->s_addr )
	{
		input[0] = srcIp()->s_addr;
		input[1] = dstIp()->s_addr;
		input[2] = (uint32_t)srcPort() << 16 | ((uint32_t)dstPort() & 0x0000FFFF);

		input[3] = protocol();
	}
	else if (srcIp()->s_addr > dstIp()->s_addr)
	{
		input[0] = dstIp()->s_addr;
		input[1] = srcIp()->s_addr;
		input[2] = (uint32_t)dstPort() << 16 | ((uint32_t)srcPort() & 0x0000FFFF);

		input[3] = protocol();
	}
	else
	{
		SLOG_INFO( << "sp : " << srcPort() << " dp " << dstPort());
		
		if( srcPort() <= dstPort() )
		{
			input[0] = srcIp()->s_addr;
			input[1] = dstIp()->s_addr;
			input[2] = (uint32_t)srcPort() << 16 | ((uint32_t)dstPort() & 0x0000FFFF);

			input[3] = protocol();
		}
		else
		{
			input[0] = dstIp()->s_addr;
			input[1] = srcIp()->s_addr;
			input[2] = (uint32_t)dstPort() << 16 | ((uint32_t)srcPort() & 0x0000FFFF);

			input[3] = protocol();
		}
	}

	SLOG_INFO(<< " : input 0 = " << input[0]
				<< " : input 1 = " << input[1]
				<< " : input 2 = " << input[2]
				<< " : input 3 = " << input[3]
				<< " : sizeof = " << sizeof(input));

	//void MurmurHash3_x86_32 (const void *key, int len, uint32_t seed, void *out);
	uint32_t hash = 0;
	MurmurHash3_x86_32(input, sizeof(input), gvMurmurSeed, &hash);
	return hash;

}

bool IPv4::sameFlow(IPv4& rhs)
{
	if(protocol() != rhs.protocol())
		return false;

	if(srcIp()->s_addr == rhs.srcIp()->s_addr 
			&& dstIp()->s_addr == rhs.dstIp()->s_addr
			&& srcPort() == rhs.srcPort()
			&& dstPort() == rhs.dstPort())
	{
		// same flow and direction
		return true;
	}
	else if( srcIp()->s_addr == rhs.dstIp()->s_addr 
			&& dstIp()->s_addr == rhs.srcIp()->s_addr
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