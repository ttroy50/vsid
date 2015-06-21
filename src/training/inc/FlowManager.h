/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_FLOW_MANAGER_H__
#define __VSID_FLOW_MANAGER_H__

#include <memory>
#include <unordered_set>

#include "IPv4.h"
#include "Flow.h"
#include "Hasher.h"

namespace VSID_TRAINING
{

template <> 
inline uint32_t Ipv4FlowHasher::operator()(const Flow* t) const
{
	return (*this)(&(t->fiveTuple()));
}

class FlowPtrEqualFn
{
public:
  bool operator() (std::shared_ptr<Flow> const& t1, std::shared_ptr<Flow> const& t2) const
  {
    return (*t1 == *t2);
  }
};

class FlowManager
{
public:
	static FlowManager* getInstance();

	/**
	 * Add a new packet to a flow. This will create a new flow it it doesn't 
	 * exist and will attempt to start the lookup
	 * @param  packet [description]
	 * @return        The flow in use or NULL if one could not be created
	 */
	std::shared_ptr<Flow> addPacket(IPv4Packet* packet);

	/**
	 * Check if a flow exists
	 * @param  packet [description]
	 * @return        [description]
	 */
	bool flowExists(IPv4Packet* packet);

	/**
	 * Finds if a flow exists in the manager. 
	 * 
	 * Only matches the hash and not the equality operator.
	 * @param  hash [description]
	 * @return      [description]
	 */
	bool flowExists(uint32_t hash);

	/**
	 * Get a Flow. If this is a new flow then it will be created
	 * @param  packet [description]
	 * @return        [description]
	 */
	std::shared_ptr<Flow>  getFlow(IPv4Packet* packet);

	/**
	 * Lookup a flow by it's hash. Won't create a new flow in the list
	 */
	std::shared_ptr<Flow>  getFlow(uint32_t hash);

	/**
	 * Delete a flow
	 * 
	 * @param  packet
	 * @return       
	 */
	void deleteFlow(IPv4Packet* packet);
	void deleteFlow(uint32_t hash);
	void deleteFlow(std::shared_ptr<Flow> flow);

	/**
	 * Number of flows 
	 * 
	 * @return
	 */
	size_t numFlows() { return _flows.size(); }

private:
	FlowManager();
	~FlowManager();

	// TODO remove singleton
	static FlowManager* _instance;

	typedef std::unordered_set<std::shared_ptr<Flow>, Ipv4FlowHasher, FlowPtrEqualFn> FlowSet;
	FlowSet _flows;

};

} // end namespace

#endif // END HEADER GUARD