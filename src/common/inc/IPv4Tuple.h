/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_COMMON_IPV4_TUPLE_H__
#define __VSID_COMMON_IPV4_TUPLE_H__

#include "Logger.h"
 
namespace VsidCommon
{

class IPv4Tuple 
{

public:
	/**
	 * Default constructor. Sets all items to 0 and means match any flow.
	 */
	IPv4Tuple() :
		src_ip(0),
		src_port(0),
		dst_ip(0),
		dst_port(0),
		transport(0)
	{}


	IPv4Tuple(uint32_t src_ip,
				uint16_t src_port,
				uint32_t dst_ip,
				uint16_t dst_port,
				uint8_t transport) :
		src_ip(src_ip),
		src_port(src_port),
		dst_ip(dst_ip),
		dst_port(dst_port),
		transport(transport)
	{
	}

	// functions for use with hasher
	uint32_t srcIp() const { return src_ip; } 
	uint16_t srcPort()  const { return src_port; } 
	uint32_t dstIp() const { return dst_ip; } 
	uint16_t dstPort() const { return dst_port; } 
	uint8_t protocol() const { return transport; } 


	uint32_t src_ip;
	uint16_t src_port;
	uint32_t dst_ip;
	uint16_t dst_port;
	uint8_t transport;

};

	// Make loggable
	inline MAKE_LOGGABLE(IPv4Tuple, tuple, os)
	{
		char src[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET, &tuple.src_ip, src, INET6_ADDRSTRLEN);

		char dst[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET, &tuple.dst_ip , dst, INET6_ADDRSTRLEN);

	    os << "transport : " << (uint32_t)tuple.transport 
	    	<< " | src : " << src << ":" << tuple.src_port 
	    	<< " | dst : " << dst << ":" << tuple.dst_port;
	}



}

#endif