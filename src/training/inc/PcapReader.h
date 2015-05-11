/**
 * Pcap Reader
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_TRAINING_PCAP_READER_H__
#define __VSID_TRAINING_PCAP_READER_H__

#include "pcap/pcap.h"

#include <string>

namespace VSID_TRAINING
{

class PcapReader
{
public:
	PcapReader() {};
	~PcapReader() {};

	bool read(const std::string& fileName);

	static void readPacket(u_char* userArg, const pcap_pkthdr* header, const u_char* packet);

private:
};

}

#endif