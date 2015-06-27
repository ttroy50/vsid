#include <string>
#include <iostream>
#include <cstdio>
#include <netinet/in.h>

#define BOOST_TEST_MODULE VsidCommonTest
#include <boost/test/unit_test.hpp>

#include "Logger.h"
#include "IPv4Tuple.h"
#include "Hasher.h"
#include "Flow.h"

using namespace VsidCommon;
using namespace std;

INIT_LOGGING

BOOST_AUTO_TEST_SUITE( tuple )


BOOST_AUTO_TEST_CASE( test_tuple_directions )
{   
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



BOOST_AUTO_TEST_SUITE( flow )


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

    BOOST_CHECK_EQUAL( flow.packetDirection(&ip_tcp), Flow::ORIG_TO_DEST );
    BOOST_CHECK_EQUAL( flow.packetDirection(&ip_tcp_opposite), Flow::DEST_TO_ORIG );
}   

BOOST_AUTO_TEST_SUITE_END()