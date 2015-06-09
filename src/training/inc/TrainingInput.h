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
 
namespace VSID_TRAINING
{

enum Protocol {
	UNKNOWN,
	HTTP,
	HTTPS,
	SIP
};

class TrainingFlow
{
public:
	TrainingFlow() : 
		processed(false), 
		protocol(UNKNOWN) 
	{}

	IPv4Tuple tuple;
	Protocol protocol;
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
	bool read(const std::string& filename);


	static uint8_t strToTransport(const std::string& str);
	static Protocol strToProtocol(const std::string& str);

	std::vector<TrainingFile>& trainingFiles() { return _trainingFiles; }
private:
	std::vector<TrainingFile> _trainingFiles;
};


}
#endif