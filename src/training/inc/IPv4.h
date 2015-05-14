#ifndef __VSID_IPV4_H__
#define __VSID_IPV4_H__

#include <sys/types.h>
#include <netinet/ip.h>

namespace VSID_TRAINING
{


class IPv4
{
public:
	IPv4(const u_char* pkt, int pkt_size, const u_char* ip_hdr_start,
				 const u_char* transport_hdr_start, const u_char* data_start) :
		_pkt(pkt),
		_pkt_size(pkt_size),
		_ip_hdr_start(ip_hdr_start),
		_transport_hdr_start(transport_hdr_start),
		_data_start(data_start)
	{}

	virtual ~IPv4() {};

	const ip* iphdr() { return (struct ip*)_ip_hdr_start; }
	const in_addr* srcIp() { return &iphdr()->ip_src; }
	//std::string srcIpString();

	const in_addr* dstIp() { return &iphdr()->ip_dst; };
	//std::string srcIpString();

	uint8_t protocol() { return iphdr()->ip_p; };

	const u_char* transport_hdr_start() { return _transport_hdr_start; }
	virtual uint16_t srcPort() { return 0; }
	virtual uint16_t dstPort() { return 0; }


	const u_char* data_start() { return _data_start; }

	uint32_t flowHash();

	bool sameFlow(IPv4& rhs);


protected:
	const u_char* _pkt;
	int _pkt_size;
	const u_char* _ip_hdr_start;
	const u_char* _transport_hdr_start;
	const u_char* _data_start;

};



}

#endif