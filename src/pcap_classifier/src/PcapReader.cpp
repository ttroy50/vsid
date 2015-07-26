

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include "pcap/pcap.h"

#include "PcapReader.h"
#include "Logger.h"
#include "Config.h"
#include "Constants.h"
#include "TcpIpv4.h"
#include "UdpIpv4.h"


#include "FlowManager.h"

using namespace std;
using namespace VsidPcapClassifier;
using namespace VsidCommon;

struct CbData
{
	PcapReader* reader;
	pcap_t* pcap;
};

void readPacket(u_char* userArg, 
							const pcap_pkthdr* pkthdr,
							const u_char* packet)
{
    CbData* cbData = reinterpret_cast<CbData*>(userArg);
    cbData->reader->handlePacket(cbData->pcap, pkthdr, packet);
}


void PcapReader::handlePacket(pcap_t* pcap,
							const pcap_pkthdr* pkthdr,
							const u_char* packet)
{
    _numPackets++;
    SLOG_INFO( << "Packet count : " << _numPackets);
    //LOG_HEXDUMP("Packet :", packet, pkthdr->len)
   	SLOG_INFO( << "ts.sec : " << pkthdr->ts.tv_sec << " ts.usec : " << pkthdr->ts.tv_usec);
    SLOG_INFO( << "caplen : " << pkthdr->caplen);
    SLOG_INFO( << "len : " << pkthdr->len);

    int linktype;

    uint32_t linkhdrlen = 14;
 
    // Determine the datalink layer type.
    if ((linktype = pcap_datalink(pcap)) < 0)
    {
        LOG_ERROR(("pcap_datalink(): %v\n", pcap_geterr(pcap)));
        return;
    }
 
    // Set the datalink layer header size.
    switch (linktype)
    {
    case DLT_NULL:
        linkhdrlen = 4;
        SLOG_INFO(<< "DLT_NULL");
        break;
 
    case DLT_EN10MB:
        linkhdrlen = LL_ETH_HDR_LEN;
        SLOG_INFO(<< "DLT_EN10MB");
        break;
 
    case DLT_SLIP:
    case DLT_PPP:
        linkhdrlen = 24;
        SLOG_INFO(<< "DLT_PPP");
        break;
 
 	case DLT_LINUX_SLL:
 		linkhdrlen = LL_SLL_HDR_LEN;
 		SLOG_INFO(<< "LL_SLL_HDR_LEN");
 		break;
    default:
        LOG_ERROR(("Unsupported datalink (%v)\n", linktype));
        return;
    }

    u_char* myPacketCpy = new u_char[pkthdr->len];
    memcpy(myPacketCpy, packet, pkthdr->len);


    struct ip_vhl {
		unsigned int ip_hl:4; // only in IPv4
		unsigned int ip_v:4;
    };	

    // IP Header
    const u_char* ip_hdr_start = myPacketCpy + linkhdrlen;

    ip_vhl* vhl = (struct ip_vhl*)(ip_hdr_start);
    SLOG_INFO(<< "IP - HLen : " << vhl->ip_hl * 4 << " - Ver : " << vhl->ip_v);

    if(vhl->ip_v == IPv4)
    {
		struct iphdr* ip_hdr = (struct iphdr*)(ip_hdr_start);
		const u_char* transport_hdr_start = ip_hdr_start + (vhl->ip_hl * 4);

		// extract the source and the destination ip-adrress
		in_addr* srcIp = (in_addr*) &ip_hdr->saddr;
		in_addr* dstIp = (in_addr*) &ip_hdr->daddr;

		// convert datagram network byte order to host byte order and calculate
		// the header size
		//uint16_t ipDatagramSize = ntohs(ip_hdr->ip_len);
		//uint16_t headerSize     = IP_HL(ip_hdr) << 2;

		char src[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET, srcIp, src, INET6_ADDRSTRLEN);

		char dst[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET, dstIp, dst, INET6_ADDRSTRLEN);

		SLOG_INFO( << "src ip : " << src );
		SLOG_INFO( << "dst ip : " << dst );

		// Transport Layer Header
		switch(ip_hdr->protocol)
		{
			case IPPROTO_ICMP:
			{
				SLOG_INFO( << "IPPROTO_ICMP");
				break;
			}
			case IPPROTO_TCP:
			{
				SLOG_INFO( << "IPPROTO_TCP");

				struct tcphdr* tcph = (struct tcphdr*)(transport_hdr_start);

				SLOG_INFO(<< "tcph->doff : " << tcph->doff);
				const u_char* data_start = transport_hdr_start  + tcph->doff * 4;
				
				TcpIPv4* tcp = new TcpIPv4(myPacketCpy, pkthdr->len, ip_hdr_start, 
                							transport_hdr_start, data_start, 
                							pkthdr->ts,
                                            myPacketCpy);

				_flowManager->addPacket(tcp);

				break;
			}
			case IPPROTO_UDP:
			{
				SLOG_INFO( << "IPPROTO_UDP");

				const u_char* data_start = transport_hdr_start + sizeof(udphdr);

				UdpIPv4* udp = new UdpIPv4(myPacketCpy, pkthdr->len, ip_hdr_start, 
            							transport_hdr_start, data_start, 
            							pkthdr->ts,
                                        myPacketCpy);

				_flowManager->addPacket(udp);

				break;
			}
			case IPPROTO_SCTP:
			{
				SLOG_INFO( << "IPPROTO_SCTP");
				break;
			}
			default:
			{
				SLOG_INFO( << "UNKNOWN IPPROTO : " << ip_hdr->protocol);
			}

		}
	}
	else if(vhl->ip_v == IPv6)
	{
		SLOG_ERROR(<< "IP Version 6 not supported yet")
		return;
	}
	else
	{
		SLOG_ERROR(<< "Invalid IP Version : " << vhl->ip_v)
		return;
	}
}

bool PcapReader::read(const string& fileName)
{
	SLOG_INFO( << "opening pcap [" << fileName << "]");

	char errbuff[PCAP_ERRBUF_SIZE];

	pcap_t * pcap = pcap_open_offline(fileName.c_str(), errbuff);

	if(pcap == NULL)
	{
		SLOG_ERROR(<< "ERROR opening pcap [" << fileName << "] : " << errbuff);
		return false;
	}

	struct bpf_program  filter;


	 // compiles the filter expression into a BPF filter program.
    // ip and (tcp or udp) filters only ip version 4 packets with tcp or udp
    if (pcap_compile(pcap, &filter,
                     "ip and (tcp or udp)", 1, 0) == -1) {
        cerr << "Error bpf: " << pcap_geterr(pcap) << endl;
        return false;
    }

    // load the filter program into the packet capture device.
    if (pcap_setfilter(pcap, &filter) == -1) {
        cerr << "Error filter: " << pcap_geterr(pcap) << endl;
        return false;
    }

    std::shared_ptr<CbData> cbData (new CbData);
    cbData->reader = this;
    cbData->pcap = pcap;

	pcap_loop(pcap, 0, &readPacket, reinterpret_cast<u_char*>(cbData.get()));

    pcap_freecode(&filter);
    pcap_close(pcap);
	return true;
}
