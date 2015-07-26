#ifndef __VSID_UDP_IPV4_H__
#define __VSID_UDP_IPV4_H__

#include "IPv4.h"
#include <netinet/ip.h>
#include <netinet/udp.h>


namespace VsidCommon
{


class UdpIPv4 : public IPv4Packet
{
public:
	UdpIPv4(const u_char* pkt, 
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

	UdpIPv4(const u_char* pkt, 
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

	virtual ~UdpIPv4() {};


	const struct udphdr* udphdr() const { return (const struct udphdr*)_transport_hdr_start; }

	virtual uint16_t srcPort() const { return  ntohs(udphdr()->uh_sport); }
	
	virtual uint16_t dstPort() const { return ntohs(udphdr()->uh_dport); }

	uint16_t udp_len() { return udphdr()->len; }

protected:

};



} // end namespace

#endif // END HEADER GUARD