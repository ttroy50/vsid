/**
 * Training Input
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_TRAINING_TRAINING_INPUT_H__
#define __VSID_TRAINING_TRAINING_INPUT_H__

namespace VSID_TRAINING
{

enum Protocol {
	HTTP,
	HTTPS,
	SIP
}

struct IPv4Tuple {
	/**
	 * Default constructor. Sets all items to 0 and means match any flow.
	 */
	IPv4Tuple();


	IPv4Tuple(uint32_t src_ip,
				uint16_t src_port,
				uint32_t dst_ip,
				uint16_t dst_port,
				uint8_t transport);


	uint32_t src_ip;
	uint16_t src_port;
	uint32_t dst_ip;
	uint16_t dst_port;
	uint8_t transport;
};

struct TrainingFlow
{
	IPv4Tuple tuple;
	Protocol protocol;
};

struct TrainingFile {
	std::string& filename;
	std::vector<TrainingFlow> flows;
};

class TrainingInput
{
public:
	bool read(const std::string& filename);


private:
	std::vector<TrainingFile> pcapfiles;
};


}
#endif