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

namespace VsidCommon
{

class IPv4Packet
{
public:
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
		_copy_packet(false),
		_timestamp(ts)
	{
		//const u_char* _pkt;
	}

	IPv4Packet(const u_char* pkt, 
			int pkt_size, 
			const u_char* ip_hdr_start,
			const u_char* transport_hdr_start, 
			const u_char* data_start,
			bool copy_packet,
			struct timeval ts) :
		_copy_packet(copy_packet),
		_pkt_size(pkt_size),
		_ip_hdr_start(ip_hdr_start),
		_transport_hdr_start(transport_hdr_start),
		_data_start(data_start),
		_timestamp(ts)
	{
		u_char* tmppkt = (u_char*) malloc(pkt_size*sizeof(u_char));
		memcpy(tmppkt, pkt, pkt_size*sizeof(u_char));
		_pkt = tmppkt;
	}

	virtual ~IPv4Packet() 
	{
		if(_copy_packet)
			free((void*)_pkt);
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

	uint32_t flowHash();

	bool sameFlow(IPv4Packet& rhs);

	const struct timeval& timestamp() const { return _timestamp; }

protected:
	const u_char* _pkt;
	int _pkt_size;
	const u_char* _ip_hdr_start;
	const u_char* _transport_hdr_start;
	const u_char* _data_start;
	bool _copy_packet;
	
	// Timestamp for when the packet was created
	struct timeval _timestamp;
};

} // end namespace

#endif // END HEADER GUARD