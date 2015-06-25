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

namespace VsidTraining
{

class PcapReader
{
public:
	PcapReader() {};
	~PcapReader() {};

	/**
	 * read a pcap file
	 * @param  fileName
	 * @return
	 */
	bool read(const std::string& fileName);

	/**
	 * read a packet coming from the pcap_loop callback.
	 *
	 *
	 * @param userArg cast of the pcap* pointer
	 * @param header 
	 * @param packet 
	 */
	static void readPacket(u_char* userArg, const pcap_pkthdr* header, const u_char* packet);

private:
};

}

#endif