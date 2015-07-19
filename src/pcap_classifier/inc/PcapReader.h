/**
 * Pcap Reader
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_PCAP_CLASSIFIER_PCAP_READER_H__
#define __VSID_PCAP_CLASSIFIER_PCAP_READER_H__

#include "pcap/pcap.h"

#include <string>

#include "FlowManager.h"

namespace VsidPcapClassifier
{

class PcapReader
{
public:
	PcapReader(VsidCommon::FlowManager* flowManager) :
		_flowManager(flowManager),
		_numPackets(0)
	{};
	~PcapReader() {};

	/**
	 * read a pcap file
	 * @param  fileName
	 * @return
	 */
	bool read(const std::string& fileName);

	/**
	 * Handle a packet
	 *
	 * @param pcap   
	 * @param header 
	 * @param packet 
	 */
	void handlePacket(pcap_t* pcap, const pcap_pkthdr* header, const u_char* packet);
private:

	uint64_t _numPackets;
	
	VsidCommon::FlowManager* _flowManager;

};

}

#endif