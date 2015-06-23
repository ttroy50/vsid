/**
 * Pcap Reader
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_NETFILTER_PACKET_HANDLER_H__
#define __VSID_NETFILTER_PACKET_HANDLER_H__

#include <string>

namespace VsidNetfilter
{

class PacketHandler
{
public:
	PacketHandler() {};
	~PacketHandler() {};

	void run();

private:
};

}

#endif