#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "pcap/pcap.h"

#include "PcapReader.h"
#include "Logger.h"
#include "Config.h"
#include "Constants.h"


using namespace std;
using namespace VSID_TRAINING;

void PcapReader::readPacket(u_char* userArg, const pcap_pkthdr* pkthdr, const u_char* packet)
{

    static int count = 1;
    SLOG_INFO( << "count : " << count << endl
   				<< "\tts.sec : " << pkthdr->ts.tv_sec << " ts.usec : " << pkthdr->ts.tv_usec << endl
    			<< "\t\t\tcaplen : " << pkthdr->caplen << endl
    			<< "\t\t\tlen : " << pkthdr->len << endl);

    pcap_t* pcap = (pcap_t*)userArg;
    int linktype;
    int linkhdrlen = 14;
 
    // Determine the datalink layer type.
    if ((linktype = pcap_datalink(pcap)) < 0)
    {
        printf("pcap_datalink(): %s\n", pcap_geterr(pcap));
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
        printf("Unsupported datalink (%d)\n", linktype);
        return;
    }

	struct ip* p_ip_header = (struct ip*)(packet + linkhdrlen);

	// extract the source and the destination ip-adrress
	in_addr* srcIp = (in_addr*) &p_ip_header->ip_src;
    in_addr* dstIp = (in_addr*) &p_ip_header->ip_src;

    // convert datagram network byte order to host byte order and calculate
    // the header size
    //uint16_t ipDatagramSize = ntohs(p_ip_header->ip_len);
    //uint16_t headerSize     = IP_HL(p_ip_header) << 2;

	char str[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET, &p_ip_header->ip_src, str, INET6_ADDRSTRLEN);

	char str2[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET, dstIp, str2, INET6_ADDRSTRLEN);

    SLOG_INFO( << "srcIP : " << str << endl
    			<< "\t\t\tdstIp : " << str2 << endl);
    count++;
}

bool PcapReader::read(const string& fileName)
{

	char errbuff[PCAP_ERRBUF_SIZE];

	pcap_t * pcap = pcap_open_offline(fileName.c_str(), errbuff);

	struct bpf_program  filter;
	
	 // compiles the filter expression into a BPF filter program.
    // ip and (tcp or udp) filters only ip version 4 packets with tcp or udp
    if (pcap_compile(pcap, &filter,
                     "ip and (tcp or udp)", 1, 0) == -1) {
        cerr << "Error bpf: " << pcap_geterr(pcap) << endl;
        exit(EXIT_FAILURE);
    }

    // load the filter program into the packet capture device.
    if (pcap_setfilter(pcap, &filter) == -1) {
        cerr << "Error filter: " << pcap_geterr(pcap) << endl;
        exit(EXIT_FAILURE);
    }


	if(pcap == NULL)
	{
		SLOG_ERROR(<< "ERROR opening pcap [" << fileName << "] : " << errbuff);
		return false;
	}

	pcap_loop(pcap, 0, readPacket, (u_char*)pcap);

	pcap_close(pcap);

	return true;
}