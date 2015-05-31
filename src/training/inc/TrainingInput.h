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

namespace VSID_TRAINING
{

enum Protocol {
	UNKNOWN,
	HTTP,
	HTTPS,
	SIP
};

class IPv4Tuple {
public:
	/**
	 * Default constructor. Sets all items to 0 and means match any flow.
	 */
	IPv4Tuple() {}


	IPv4Tuple(uint32_t src_ip,
				uint16_t src_port,
				uint32_t dst_ip,
				uint16_t dst_port,
				uint8_t transport) {}


	uint32_t src_ip;
	uint16_t src_port;
	uint32_t dst_ip;
	uint16_t dst_port;
	uint8_t transport;
};

class TrainingFlow
{
public:
	TrainingFlow() : processed(false) {}

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