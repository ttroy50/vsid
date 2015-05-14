#ifndef __VSID_TCP_IPV4_H__
#define __VSID_TCP_IPV4_H__

#include "IPv4.h"
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include "Logger.h"
namespace VSID_TRAINING
{


class TcpIPv4 : public IPv4
{
public:
	TcpIPv4(const u_char* pkt, int pkt_size, const u_char* ip_hdr_start, 
					const u_char* transport_hdr_start, const u_char* data_start) :
		IPv4(pkt, pkt_size, ip_hdr_start, transport_hdr_start, data_start)
	{};
	virtual ~TcpIPv4() {};


	const struct tcphdr* tcphdr() { return (const struct tcphdr*)_transport_hdr_start; }

	virtual uint16_t srcPort() { SLOG_INFO(<< "heres"); return  ntohs(tcphdr()->th_sport); }
	virtual uint16_t dstPort() { SLOG_INFO(<< "hered"); return ntohs(tcphdr()->th_dport); }

protected:

};



}

#endif