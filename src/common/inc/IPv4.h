/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_IPV4__H__
#define __VSID_IPV4__H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  
#include <sys/types.h>
#include <netinet/ip.h>
#include "Logger.h"

namespace VsidCommon
{

class IPv4Packet
{
public:
	/**
	 * Packet where the receiving buffer is owned by the caller.
	 */
	IPv4Packet(const u_char* pkt, 
			int pkt_size, 
			const u_char* ip_hdr_start,
			const u_char* transport_hdr_start, 
			const u_char* data_start,
			struct timeval ts) :
		_pkt(pkt),
		_pkt_size(pkt_size),
		_ip_hdr_start(ip_hdr_start),
		_transport_hdr_start(transport_hdr_start),
		_data_start(data_start),
		_timestamp(ts),
		_buffer(NULL)
	{
	}

	/**
	 * Constructor that gives this packet ownership of the buffer it was received on
	 */
	IPv4Packet(const u_char* pkt, 
			int pkt_size, 
			const u_char* ip_hdr_start,
			const u_char* transport_hdr_start, 
			const u_char* data_start,
			struct timeval ts,
			u_char* buffer) :
		_pkt(pkt),
		_pkt_size(pkt_size),
		_ip_hdr_start(ip_hdr_start),
		_transport_hdr_start(transport_hdr_start),
		_data_start(data_start),
		_timestamp(ts),
		_buffer(buffer)
	{
		SLOG_INFO(<< "Packet deleted");
	}

	virtual ~IPv4Packet() 
	{
		if(_buffer)
		{
			delete[] _buffer;
		}
	}

	const struct iphdr* iphdr() const { return (const struct iphdr*)_ip_hdr_start; }

	uint16_t version() const { return iphdr()->version; }

	uint16_t ip_len() const { return iphdr()->ihl; }

	uint32_t srcIp() const { return iphdr()->saddr; }
	//std::string srcIpString();

	uint32_t dstIp() const { return iphdr()->daddr; };
	//std::string srcIpString();

	uint8_t protocol() const { return iphdr()->protocol; };

	const u_char* transport_hdr_start() { return _transport_hdr_start; }

	virtual uint16_t srcPort() const { return 0; }
	virtual uint16_t dstPort() const { return 0; }


	const u_char* data() { return _data_start; }
	size_t dataSize() { return _pkt + _pkt_size - _data_start; }

	uint32_t flowHash();

	bool sameFlow(IPv4Packet& rhs);

	const struct timeval& timestamp() const { return _timestamp; }

protected:
	const u_char* _pkt;
	int _pkt_size;
	const u_char* _ip_hdr_start;
	const u_char* _transport_hdr_start;
	const u_char* _data_start;
	// The buffer we received the packet on (if we own it)
	u_char* _buffer;
	
	// Timestamp for when the packet was created
	struct timeval _timestamp;
};

} // end namespace

#endif // END HEADER GUARD