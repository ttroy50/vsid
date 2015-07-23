#include <string>
#include <iostream>
#include <cstdio>
#include <netinet/in.h>

#include "AttributeMeterFactory.h"
#include "AttributeMeterRegistrar.h"

#define BOOST_TEST_MODULE VsidCommonTest
#include <boost/test/unit_test.hpp>

#include "Logger.h"
#include "IPv4Tuple.h"
#include "Hasher.h"
#include "Flow.h"
#include "TcpIpv4.h"

#include <regex>


using namespace VsidCommon;
using namespace Vsid;
using namespace std;

INIT_LOGGING

int hexToData(const char* hex, unsigned char** data)
{
    // create the packet using hexdump from the test file

    if (hex == NULL) {
        return -1;
    }
    int first = 1;
    const char* ch = hex;
    unsigned char c = 0;

    int len = strlen(hex);
    int data_len = 0;
    *data = (uint8_t*)malloc(len/2);
    char *d_p = (char*)*data;
    while (ch && (ch-hex) < len) {
        switch (*ch) {

            case ' ':  // space
            case '\t': // tab
            case '\n': // newline
                // skip over
                break;

            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '0':
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':

                if (*ch >= '0' && *ch <= '9') {
                    c = (unsigned char) *ch - '0';
                } else { // must be hex letter
                    switch (*ch) {
                        case 'a': case 'A': c = 0x0a; break;
                        case 'b': case 'B': c = 0x0b; break;
                        case 'c': case 'C': c = 0x0c; break;
                        case 'd': case 'D': c = 0x0d; break;
                        case 'e': case 'E': c = 0x0e; break;
                        case 'f': case 'F': c = 0x0f; break;
                    }
                }

                if (first != 1) {
                    *d_p = *d_p | (c & 0x0f);
                    first = 1;
                    data_len++;
                    d_p++; // move to next char
                } else {
                    *d_p = c << 4;
                    first = 0;
                }
                break;
            default: // treat everything else as a comment and read till EOL
                while (ch && *ch != '\n' && *ch != EOF && *ch != '\0') {
                    ++ch;
                    if (ch == NULL)
                        continue;
                }
                break;
        }
        ++ch;
    }

    if (first != 1) {
        std::cout << "Test::hexToData: ERROR: uneven number of chars in data" << std::endl;
        return -1;
    }

    return data_len;
}



BOOST_AUTO_TEST_SUITE( tuple )


BOOST_AUTO_TEST_CASE( test_tuple_directions )
{   
    init_attribute_meters();

    IPv4Tuple ip_tcp(169411074, 5555, 1499754872, 80, IPPROTO_TCP);
    IPv4Tuple ip_tcp_opposite(1499754872, 80, 169411074, 5555, IPPROTO_TCP);

    Ipv4FlowHasher hasher;

    uint32_t ip_tcp_hash = hasher(&ip_tcp);
    uint32_t ip_tcp_opposite_hash = hasher(&ip_tcp_opposite);

    BOOST_CHECK_EQUAL( ip_tcp_hash, ip_tcp_opposite_hash );

}

BOOST_AUTO_TEST_CASE( test_tuple_port_change )
{   
    IPv4Tuple ip_tcp(169411074, 5555, 1499754872, 80, IPPROTO_TCP);
    IPv4Tuple ip_tcp_opposite(1499754872, 5555, 169411074, 80, IPPROTO_TCP);

    Ipv4FlowHasher hasher;

    uint32_t ip_tcp_hash = hasher(&ip_tcp);
    uint32_t ip_tcp_opposite_hash = hasher(&ip_tcp_opposite);

    BOOST_CHECK( ip_tcp_hash != ip_tcp_opposite_hash );

}

BOOST_AUTO_TEST_CASE( test_tuple_same_ip )
{	
	IPv4Tuple same_ip_tcp(1499754872, 5555, 1499754872, 80, IPPROTO_TCP);
    IPv4Tuple same_ip_tcp_opposite(1499754872, 80, 1499754872, 5555, IPPROTO_TCP);

    Ipv4FlowHasher hasher;

    uint32_t same_ip_tcp_hash = hasher(&same_ip_tcp);
    uint32_t same_ip_tcp_opposite_hash = hasher(&same_ip_tcp_opposite);

    BOOST_CHECK_EQUAL( same_ip_tcp_hash, same_ip_tcp_opposite_hash );

    IPv4Tuple same_ip_udp(1499754872, 5555, 1499754872, 80, IPPROTO_UDP);
    IPv4Tuple same_ip_udp_opposite(1499754872, 80, 1499754872, 5555, IPPROTO_UDP);

    uint32_t same_ip_udp_hash = hasher(&same_ip_udp);
    uint32_t same_ip_udp_opposite_hash = hasher(&same_ip_udp_opposite);

    BOOST_CHECK_EQUAL( same_ip_udp_hash, same_ip_udp_opposite_hash );
    cout << "udp [" << same_ip_udp_hash << "] : tcp [" << same_ip_tcp_hash << "]" << endl;
    BOOST_CHECK( same_ip_udp_hash != same_ip_tcp_hash);

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( IPv4 )

BOOST_AUTO_TEST_CASE( tcp )
{
    timeval tv;

    // SYN packet
    string packetAsHex = "4500003c065c40004006bcec0a0a0a8f4a7d185e9d6d01bb85f18d4800000000a00272101a9f0000020405b40402080a06668b140000000001030307";
    u_char* packet;
    int packetLen = hexToData(packetAsHex.c_str(), &packet);

    BOOST_REQUIRE( packetLen > 0 );

    u_char* tph = packet+sizeof(iphdr);
    u_char* data = tph + sizeof(tcphdr);

    TcpIPv4 tcpPacket( packet, packetLen, packet, tph, data, tv);

    BOOST_CHECK_EQUAL( tcpPacket.version(), 4);
    BOOST_CHECK_EQUAL( tcpPacket.protocol(), IPPROTO_TCP);
    BOOST_CHECK_EQUAL( ntohl(tcpPacket.srcIp()), 168430223);
    BOOST_CHECK_EQUAL( ntohl(tcpPacket.dstIp()), 1249712222);
    BOOST_CHECK_EQUAL( tcpPacket.srcPort(), 40301); 
    BOOST_CHECK_EQUAL( tcpPacket.dstPort(), 443); 
    BOOST_CHECK_EQUAL( tcpPacket.flags(), 0x02);


    // data packet in opposite direction
    string oppAsHex = "450005be300f00003106dcb74a7d185e0a0a0a8f01bb9d6d2a2d705f85f18e148010015580f800000101080ae72e6c1506668b18160303004a020000460303558f061e8bb498e15dc190b8735fcf61ebb967475499f54f88fd3c60d6d3b05f00c02b00001e00000000ff01000100000b000403000102002300000010000500030268321603030e4e0b000e4a000e470006c9308206c5308205ada00302010202085735d4ddb82628fe300d06092a864886f70d01010b05003049310b300906035504061302555331133011060355040a130a476f6f676c6520496e63312530230603550403131c476f6f676c6520496e7465726e657420417574686f72697479204732301e170d3135303631383039343333325a170d3135303931363030303030305a3066310b30090603550406130255533113301106035504080c0a43616c69666f726e69613116301406035504070c0d4d6f756e7461696e205669657731133011060355040a0c0a476f6f676c6520496e633115301306035504030c0c2a2e676f6f676c652e636f6d3059301306072a8648ce3d020106082a8648ce3d03010703420004aa22a75feb0281b6e25e7c5073930e638390958ed84573427276e98a6dd1512411bc29b25d11e5dea9cb080613b1e7177611edc0ee94de99cdce41c930a2ef48a382045d30820459301d0603551d250416301406082b0601050507030106082b06010505070302308203260603551d110482031d30820319820c2a2e676f6f676c652e636f6d820d2a2e616e64726f69642e636f6d82162a2e617070656e67696e652e676f6f676c652e636f6d82122a2e636c6f75642e676f6f676c652e636f6d82162a2e676f6f676c652d616e616c79746963732e636f6d820b2a2e676f6f676c652e6361820b2a2e676f6f676c652e636c820e2a2e676f6f676c652e636f2e696e820e2a2e676f6f676c652e636f2e6a70820e2a2e676f6f676c652e636f2e756b820f2a2e676f6f676c652e636f6d2e6172820f2a2e676f6f676c652e636f6d2e6175820f2a2e676f6f676c652e636f6d2e6272820f2a2e676f6f676c652e636f6d2e636f820f2a2e676f6f676c652e636f6d2e6d78820f2a2e676f6f676c652e636f6d2e7472820f2a2e676f6f676c652e636f6d2e766e820b2a2e676f6f676c652e6465820b2a2e676f6f676c652e6573820b2a2e676f6f676c652e6672820b2a2e676f6f676c652e6875820b2a2e676f6f676c652e6974820b2a2e676f6f676c652e6e6c820b2a2e676f6f676c652e706c820b2a2e676f6f676c652e707482122a2e676f6f676c656164617069732e636f6d820f2a2e676f6f676c65617069732e636e82142a2e676f6f676c65636f6d6d657263652e636f6d82112a2e676f6f676c65766964656f2e636f6d820c2a2e677374617469632e636e820d2a2e677374617469632e636f6d820a2a2e677674312e636f6d820a2a2e677674322e636f6d82142a2e6d65747269632e677374617469632e636f6d820c2a2e75726368696e2e636f6d82102a2e75726c2e676f6f676c652e636f6d82162a2e796f75747562652d6e6f636f6f6b69652e636f6d820d2a2e796f75747562652e636f6d82162a2e796f7574756265656475636174696f6e2e636f6d820b2a2e7974696d672e636f6d820b616e64726f69642e636f6d8204672e636f8206676f6f2e676c8214676f6f676c652d616e616c79746963732e636f6d820a676f6f676c652e636f6d8212676f6f676c65636f6d6d657263652e636f6d820a75726368696e2e636f6d8208796f7574752e6265820b796f75747562652e636f6d8214796f7574756265656475636174696f6e2e636f6d300b0603551d0f040403020780306806082b06010505070101045c305a302b06082b06010505073002861f687474703a2f2f706b692e676f6f676c652e636f6d2f47494147322e637274302b06082b06010505073001861f687474703a2f2f636c69656e7473312e676f6f676c652e636f6d2f6f637370301d0603551d0e0416041400703b2f2c";
    u_char* oppPacket;
    int oppPacketLen = hexToData(oppAsHex.c_str(), &oppPacket);

    BOOST_REQUIRE( oppPacketLen > 0 );

    tph = oppPacket+sizeof(iphdr);
    data = tph + sizeof(tcphdr);

    TcpIPv4 oppTcpPacket( oppPacket, oppPacketLen, oppPacket, tph, data, tv);

    BOOST_CHECK_EQUAL( oppTcpPacket.version(), 4);
    BOOST_CHECK_EQUAL( oppTcpPacket.protocol(), IPPROTO_TCP);
    BOOST_CHECK_EQUAL( ntohl(oppTcpPacket.dstIp()), 168430223);
    BOOST_CHECK_EQUAL( ntohl(oppTcpPacket.srcIp()), 1249712222);
    BOOST_CHECK_EQUAL( oppTcpPacket.dstPort(), 40301); 
    BOOST_CHECK_EQUAL( oppTcpPacket.srcPort(), 443); 
    BOOST_CHECK_EQUAL( oppTcpPacket.flags(), 0x10);


    BOOST_CHECK_EQUAL( tcpPacket.flowHash(), oppTcpPacket.flowHash());
    BOOST_CHECK( tcpPacket.sameFlow(oppTcpPacket));

    free(packet);
    free(oppPacket);
}

BOOST_AUTO_TEST_CASE( regex )
{
    timeval tv;

    // SYN packet
    string packetAsHex = "030000000000ba1400000000020007636f6e6e656374003ff00000000000000300036170700200067669657773730005746355726c02002872746d703a2f2f644e41652e736d6f6f746873747265616d732e74763a333636352f7669657773730004667061640100000c6361706162696c697469657300402e000000000000000b617564696f436f64656373";
    u_char* packet;
    int packetLen = hexToData(packetAsHex.c_str(), &packet);

    BOOST_REQUIRE( packetLen > 0 );

    /*u_char* packet = (u_char*)"AAA";
    int packetLen = strlen((const char*)packet);*/
    LOG_HEXDUMP("Packet :", packet, packetLen)

    BOOST_REQUIRE (std::regex_match ("subject", std::regex("(sub)(.*)") ));

    BOOST_REQUIRE(std::regex_match(packet, packet+packetLen, std::regex("\x03.+\x14.+\x02.+\x07.*(connect){0,1}.+(app){0,1}.+")));//?.+(app)?") ));

   

   free(packet);
}

BOOST_AUTO_TEST_SUITE_END()




BOOST_AUTO_TEST_SUITE( flow )

/*
BOOST_AUTO_TEST_CASE( flow_from_tuple )
{   
    IPv4Tuple ip_tcp(169411074, 5555, 1499754872, 80, IPPROTO_TCP);

    Flow flow(ip_tcp);

    Ipv4FlowHasher hasher;
    uint32_t ip_tcp_hash = hasher(&ip_tcp);

    BOOST_CHECK_EQUAL( ip_tcp_hash, flow.flowHash() );

    BOOST_CHECK( flow.sameFlow(&ip_tcp) );

    IPv4Tuple ip_tcp_opposite(1499754872, 80, 169411074, 5555, IPPROTO_TCP);

    BOOST_CHECK( flow.sameFlow(&ip_tcp_opposite) );

    BOOST_CHECK( flow.packetDirection(&ip_tcp) == Flow::Direction::ORIG_TO_DEST );
    BOOST_CHECK( flow.packetDirection(&ip_tcp_opposite) == Flow::Direction::DEST_TO_ORIG );
}   
*/

BOOST_AUTO_TEST_CASE( flow_from_ip )
{   
    timeval tv;

    // SYN packet
    string packetAsHex = "4500003c065c40004006bcec0a0a0a8f4a7d185e9d6d01bb85f18d4800000000a00272101a9f0000020405b40402080a06668b140000000001030307";
    u_char* packet;
    int packetLen = hexToData(packetAsHex.c_str(), &packet);

    BOOST_REQUIRE( packetLen > 0 );

    u_char* tph = packet+sizeof(iphdr);
    u_char* data = tph + sizeof(tcphdr);

    TcpIPv4 tcpPacket( packet, packetLen, packet, tph, data, tv);

    Flow flow(&tcpPacket, NULL);
    
    Ipv4FlowHasher hasher;

    uint32_t ip_tcp_hash = hasher(&tcpPacket);

    BOOST_CHECK_EQUAL( ip_tcp_hash, flow.flowHash() );

    BOOST_CHECK( flow.sameFlow(&tcpPacket) );

    //IPv4Tuple ip_tcp_opposite(1499754872, 80, 169411074, 5555, IPPROTO_TCP);

    //BOOST_CHECK( flow.sameFlow(&ip_tcp_opposite) );

    BOOST_CHECK( flow.packetDirection(&tcpPacket) == Flow::Direction::ORIG_TO_DEST );
    // /BOOST_CHECK_EQUAL( flow.packetDirection(&ip_tcp_opposite), Flow::DEST_TO_ORIG );
}  

BOOST_AUTO_TEST_SUITE_END()


