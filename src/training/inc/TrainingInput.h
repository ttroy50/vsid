/**
 * Training Input
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_TRAINING_TRAINING_INPUT_H__
#define __VSID_TRAINING_TRAINING_INPUT_H__

#include <string>
#include <vector>
#include "IPv4Tuple.h"
#include "ProtocolModelDb.h"

namespace VsidTraining
{

enum Protocol {
	UNKNOWN,
	HTTP,
	HTTPS,
	HTTP_PROGRESSIVE,
	RTMP,
	SIP,
	DNS
};

class TrainingFlow
{
public:
	TrainingFlow() : 
		processed(false)
	{}

	uint32_t flowHash(); 

	VsidCommon::IPv4Tuple tuple;
	std::string protocol;
	bool processed;
};

class TrainingFile {
public:
	TrainingFile() : exists(false) {}

	std::string filename;
	std::vector<TrainingFlow> flows;
	bool exists;
};

class TrainingInput
{
public:
	bool read(const std::string& filename, Vsid::ProtocolModelDb* protocolModelDb);


	static uint8_t strToTransport(const std::string& str);
	static Protocol strToProtocol(const std::string& str);
	static std::string protocolToStr(Protocol protocol);

	std::vector<TrainingFile>& trainingFiles() { return _trainingFiles; }
private:
	std::vector<TrainingFile> _trainingFiles;
};


}
#endif