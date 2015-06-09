#ifndef __VSID_FLOW_MANAGER_H__
#define __VSID_FLOW_MANAGER_H__

#include <unordered_set>

#include "IPv4.h"
#include "Flow.h"
#include "Hasher.h"

namespace VSID_TRAINING
{

class FlowManager
{
public:
	FlowManager();

	/**
	 * Add a new packet to a flow. This will create a new flow it it doesn't 
	 * exist and will attempt to star the lookup
	 * @param  packet [description]
	 * @return        The flow in use or NULL if one could not be created
	 */
	Flow* addPacket(IPv4* packet);

	/**
	 * Check if a flow exists
	 * @param  packet [description]
	 * @return        [description]
	 */
	bool flowExists(IPv4* packet);
	bool flowExists(uint32_t hash);

	/**
	 * Get a Flow. If this is a new flow then it will be created
	 * @param  packet [description]
	 * @return        [description]
	 */
	Flow* getFlow(IPv4* packet);
	Flow* getFlow(uint32_t hash);

private:
	std::unordered_set<Flow*, Ipv4FlowHasher> _flows;
};

}

#endif