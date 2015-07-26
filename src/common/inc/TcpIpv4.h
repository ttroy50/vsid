#ifndef __VSID_TCP_IPV4_H__
#define __VSID_TCP_IPV4_H__

#include "IPv4.h"
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include "Logger.h"


namespace VsidCommon
{


class TcpIPv4 : public IPv4Packet
{
public:
	TcpIPv4(const u_char* pkt, 
				int pkt_size, 
				const u_char* ip_hdr_start, 
				const u_char* transport_hdr_start, 
				const u_char* data_start,
				struct timeval ts) :
		IPv4Packet(pkt, 
			pkt_size, 
			ip_hdr_start, 
			transport_hdr_start, 
			data_start,
			ts)
	{

	};

	TcpIPv4(const u_char* pkt, 
				int pkt_size, 
				const u_char* ip_hdr_start, 
				const u_char* transport_hdr_start, 
				const u_char* data_start,
				struct timeval ts,
				u_char* buffer) :
		IPv4Packet(pkt, 
			pkt_size, 
			ip_hdr_start, 
			transport_hdr_start, 
			data_start,
			ts,
			buffer)
	{

	};

	virtual ~TcpIPv4() {};


	const struct tcphdr* tcphdr() const { return (const struct tcphdr*)_transport_hdr_start; }

	virtual uint16_t srcPort() const { return  ntohs(tcphdr()->th_sport); }

	virtual uint16_t dstPort() const { return ntohs(tcphdr()->th_dport); }

	uint32_t seq() { return tcphdr()->th_seq; }
	uint32_t ack() { return tcphdr()->th_ack; }

	uint8_t flags() { return tcphdr()->th_flags; }
	
protected:

};



}

#endif