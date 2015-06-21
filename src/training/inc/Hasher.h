/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef _VSID_HASHER_H__
#define _VSID_HASHER_H__

#include "murmur3.h"
#include <sys/types.h>

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

namespace VSID_TRAINING
{

/**
 * Generate a unique hash for a flow
 * 
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
class Ipv4FlowHasher
{
public:
	template <typename T> 
	uint32_t operator()(const T* t) const
	{
		uint32_t input[4];

		input[2] = 0;

		if( t->srcIp() < t->dstIp() )
		{
			input[0] = t->srcIp();
			input[1] = t->dstIp();
			input[2] = (uint32_t)t->srcPort() << 16 | ((uint32_t)t->dstPort() & 0x0000FFFF);

			input[3] = t->protocol();
		}
		else if (t->srcIp() > t->dstIp())
		{
			input[0] = t->dstIp();
			input[1] = t->srcIp();
			input[2] = (uint32_t)t->dstPort() << 16 | ((uint32_t)t->srcPort() & 0x0000FFFF);

			input[3] = t->protocol();
		}
		else
		{
			if( t->srcPort() <= t->dstPort() )
			{
				input[0] = t->srcIp();
				input[1] = t->dstIp();
				input[2] = (uint32_t)t->srcPort() << 16 | ((uint32_t)t->dstPort() & 0x0000FFFF);

				input[3] = t->protocol();
			}
			else
			{
				input[0] = t->dstIp();
				input[1] = t->srcIp();
				input[2] = (uint32_t)t->dstPort() << 16 | ((uint32_t)t->srcPort() & 0x0000FFFF);

				input[3] = t->protocol();
			}
		}

		//void MurmurHash3_x86_32 (const void *key, int len, uint32_t seed, void *out);
		uint32_t hash = 0;
		MurmurHash3_x86_32(input, sizeof(input), gvMurmurSeed, &hash);
		return hash;
	}

	template <typename T> 
	uint32_t operator()(std::shared_ptr<T> t) const
	{
		return (*this)(t.get());
	}

};

} // end namespace

#endif // END HEADER GUARD