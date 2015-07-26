/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_FLOW_MANAGER_H__
#define __VSID_FLOW_MANAGER_H__

#include <memory>
#include <unordered_map>
#include <mutex>

#include <boost/lockfree/queue.hpp>
#include "IPv4.h"
#include "Flow.h"
#include "Hasher.h"
#include "FlowObservers.h"

namespace Vsid
{
	class ProtocolModelDb;
}

namespace VsidCommon
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
	FlowManager(Vsid::ProtocolModelDb* datbase);
	~FlowManager();
	
	/**
	 * Initialise the flow managers thread pool
	 */
	void init();

	/**
	 * Add a new packet to a flow. This will create a new flow it it doesn't 
	 * exist 
	 * If using threading will put the packet onto the packet thread queue
	 * If not using theading will call process packet
	 * @param  packet [description]
	 * @return        The flow in use or NULL if one could not be created
	 */
	std::shared_ptr<Flow> addPacket(IPv4Packet* packet);

	/**
	 * Process Packets from a thread queue
	 * @param queue_id Id of the thread queue
	 */
	void processPackets(int queue_id);

	/**
	 * Process a single packet
	 * @param packet
	 */
	void processPacket(IPv4Packet* packet);

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

	/**
	 * Add Observer that is called when a flow is finished.
	 * 
	 * @param observer
	 */
	void addFlowFinishedObserver(FlowFinishedObserver* observer);
	
	/**
	 * Add Observer that is called when a flow is classified.
	 * 
	 * @param observer
	 */
	void addFlowClassifiedObserver(FlowClassifiedObserver* observer);
	
	/**
	 * Let the FlowManager know that we are finished so it can cleanup any flows
	 * And call the FlowFinishedObservers
	 */
	void finished();

	void shutdown() { _shutdown = true; }
	
private:
	std::unordered_map<uint32_t, std::shared_ptr<Flow> > _flows;

	std::vector<FlowFinishedObserver*> _flow_finished_observers;
	std::vector<FlowClassifiedObserver*> _flow_classified_observers;

	void notifyFlowFinished(std::shared_ptr<Flow> flow);
	void notifyFlowClassified(std::shared_ptr<Flow> flow);

	Vsid::ProtocolModelDb* _protocolModelDb;

	std::mutex _flowsMutex;
	std::vector<std::thread> _workerThreads;
	std::vector<boost::lockfree::queue<IPv4Packet*> *> _threadQueues;
	int _currentQueue;
	bool _shutdown;
};

} // end namespace

#endif // END HEADER GUARD