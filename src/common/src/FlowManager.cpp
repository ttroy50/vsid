#include <chrono>

#include "FlowManager.h"
#include "Flow.h"
#include "Logger.h"
#include "Hasher.h"
#include "IPv4.h"
#include "CommonConfig.h"

#include "ProtocolModelDb.h"

using namespace std;
using namespace VsidCommon;
using namespace Vsid;

FlowManager::FlowManager(ProtocolModelDb* database) :
	_protocolModelDb(database),
	_currentQueue(0),
	_shutdown(false),
	_finishing(false)
{
	
}

void FlowManager::init()
{
	for( int i = 0; i < CommonConfig::instance()->workerThreadsPerQueue(); i++ )
	{
		_threadQueues.push_back(new boost::lockfree::spsc_queue<IPv4Packet*, boost::lockfree::fixed_sized<true> >(CommonConfig::instance()->workerThreadQueueSize()));
		_threadNotifiers.push_back(ThreadWaiter());
		_workerThreads.push_back( std::thread(&FlowManager::processPackets, this, i) );
		
	}
}

FlowManager::~FlowManager()
{
	_shutdown = true;
	for( int i = 0; i < _workerThreads.size(); i++ )
	{
		_workerThreads[i].join();

		boost::lockfree::spsc_queue<IPv4Packet*, boost::lockfree::fixed_sized<true> > * tmp = _threadQueues[i];
		delete tmp;
	}
}

void FlowManager::processPackets(int queue_id)
{
	try
	{
		while(!_shutdown)
		{
			IPv4Packet* packet = NULL;
			if ( _threadQueues[queue_id]->pop(packet) )
			{
				if( packet == NULL )
				{
					SLOG_ERROR(<< "Packet is null from queue");
				}
				else
				{
					processPacket(packet);
				}
			}
			else
			{
				_threadNotifiers[queue_id].wait();
			}
		}
	}
	catch(std::exception& ex)
	{
		_shutdown = true;
		SLOG_ERROR(<< "exception caught in processPackets" << ex.what());
	}

	/*try
	{
		IPv4Packet* packet = NULL;
		while( _threadQueues[queue_id]->pop(packet) )
		{
			if(packet != NULL)
			{
				packet->setVerdict();
				delete packet;
			}
		}
	}
	catch(std::exception& ex)
	{
		SLOG_ERROR(<< "exception caught cleaning up processPackets" << ex.what());
	}*/

}

void FlowManager::processPacket(IPv4Packet* packet)
{
	try
	{
		std::shared_ptr<Flow> flow = getFlow(packet);

		if( flow )
		{
			bool classified = flow->flowClassified();

			flow->decPktsInQueue();
			flow->addPacket(packet);

			SLOG_INFO(<< "Packet added to flow : " << *flow);	

			if( flow->flowClassified() != classified && flow->flowClassified() )
			{
				notifyFlowClassified(flow);
			}

			if(flow->flowState() == Flow::State::FINISHED)
			{
				notifyFlowFinished(flow);
				deleteFlow(flow);
				SLOG_INFO(<< "Flow in finished state. Removing")
			}
			else if (flow->flowState() == Flow::State::FINISHED_NOTIFIED )
			{
				deleteFlow(flow);
				SLOG_INFO(<< "Flow in finished & notified state. Removing")
			}
		}
		else
		{
			SLOG_ERROR(<< "Unable to find flow");
			packet->setVerdict();
			delete packet;
		}
	}
	catch(const std::exception& e)
	{
		SLOG_ERROR(<< "Caught exception processing packet");
	}
	
}

std::shared_ptr<Flow> FlowManager::addPacket(IPv4Packet* packet)
{
	if(_shutdown)
	{
		packet->setVerdict();
		delete packet;
		return nullptr;
	}

	std::shared_ptr<Flow> flow = getFlow(packet);

	if( flow )
	{
		flow->incPktsInQueue();
		
		if( CommonConfig::instance()->workerThreadsPerQueue() >0 && _workerThreads.size() > 0)
		{
			if(flow->threadQueueId() == -1)
			{
				SLOG_INFO(<< "No queue in flow. Must be new")
				
				if(_currentQueue >= _workerThreads.size())
				{
					_currentQueue = 0;
				}
				flow->threadQueueId(_currentQueue);
				_currentQueue++;
			}

			if (_threadQueues[flow->threadQueueId()]->push(packet) )
			{
				_threadNotifiers[flow->threadQueueId()].notify();
				return flow;
			}
			else
			{
				SLOG_ERROR(<< "Unable to push packet onto queue : " << flow->threadQueueId());
				flow->decPktsInQueue();
				packet->dropPkt();
				delete packet;
				return flow;
			}
		}
		else
		{
			// Not in threaded mode
			processPacket(packet);
			return flow;
		}
	}
	else
	{
		packet->setVerdict();
		delete packet;
		return flow;
	}
}

bool FlowManager::flowExists(IPv4Packet* packet)
{
	std::lock_guard<std::mutex> guard(_flowsMutex);

	auto it = _flows.find(packet->flowHash());

	if(it == _flows.end())
	{
		return false;
	}

	return true;
}

bool FlowManager::flowExists(uint32_t hash)
{
	std::lock_guard<std::mutex> guard(_flowsMutex);

	auto it = _flows.find(hash);

	if(it == _flows.end())
	{
		return false;
	}

	return true;
}

std::shared_ptr<Flow>  FlowManager::getFlow(IPv4Packet* packet)
{
	std::lock_guard<std::mutex> guard(_flowsMutex);

	SLOG_INFO(<< _flows.size() << " flows in manager");

	auto it = _flows.find(packet->flowHash());

	if(it == _flows.end())
	{
		std::shared_ptr<Flow>  f(new Flow(packet, _protocolModelDb));
		_flows.insert(std::make_pair(f->flowHash(), f));
		SLOG_INFO(<< "New Flow added : " << *f);

		if( CommonConfig::instance()->workerThreadsPerQueue() >0 && _workerThreads.size() > 0)
		{
			// Will tell if flow is on thread
			for(int i = 0; i < _workerThreads.size(); i++)
			{
				if(_workerThreads[i].get_id() == std::this_thread::get_id() )
				{
					f->threadQueueId(i);
					break;
				}
			}

			// if flow created on main thread
			if(f->threadQueueId() == -1)
			{
				SLOG_INFO(<< "New flow so no queue in flow ")
				
				if(_currentQueue >= _workerThreads.size())
				{
					_currentQueue = 0;
				}
				f->threadQueueId(_currentQueue);
				_currentQueue++;
			}
		}
		
		return f;
	}
	else
	{
		if( packet->protocol() == IPPROTO_UDP )
		{
			std::shared_ptr<Flow> tmp = it->second;
			
			if( !tmp->havePktsInQueue() &&
				(packet->timestamp().tv_sec - tmp->lastPacketTimestamp().tv_sec ) > CommonConfig::instance()->udpFlowTimeout())
			{
				SLOG_INFO(<< "Flow [" << *tmp << "] found but past udp timeout. Removing from list and adding new one")
				notifyFlowFinished(tmp);
				_flows.erase(it);
				std::shared_ptr<Flow>  f(new Flow(packet, _protocolModelDb));
				_flows.insert(std::make_pair(f->flowHash(), f));

				if( CommonConfig::instance()->workerThreadsPerQueue() >0 && _workerThreads.size() > 0)
				{
					// Will tell if flow is on thread
					for(int i = 0; i < _workerThreads.size(); i++)
					{
						if(_workerThreads[i].get_id() == std::this_thread::get_id() )
						{
							f->threadQueueId(i);
							break;
						}
					}

					// if flow created on main thread
					if(f->threadQueueId() == -1)
					{
						SLOG_INFO(<< "No queue in flow. Must be new")
						
						if(_currentQueue >= _workerThreads.size())
						{
							_currentQueue = 0;
						}
						f->threadQueueId(_currentQueue);
						_currentQueue++;
					}
				}

				return f;
			}
		}
		else if( packet->protocol() == IPPROTO_TCP )
		{
			std::shared_ptr<Flow> tmp = it->second;
			
			if( (!tmp->havePktsInQueue() &&
					packet->timestamp().tv_sec - tmp->lastPacketTimestamp().tv_sec ) > CommonConfig::instance()->tcpFlowTimeout())
			{
				SLOG_INFO(<< "Flow [" << *tmp << "] found but past tcp timeout. Removing from list and adding new one")
				notifyFlowFinished(tmp);
				_flows.erase(it);
				std::shared_ptr<Flow>  f(new Flow(packet, _protocolModelDb));
				_flows.insert(std::make_pair(f->flowHash(), f));

				if( CommonConfig::instance()->workerThreadsPerQueue() >0 && _workerThreads.size() > 0)
				{
					// Will tell if flow is on thread
					for(int i = 0; i < _workerThreads.size(); i++)
					{
						if(_workerThreads[i].get_id() == std::this_thread::get_id() )
						{
							f->threadQueueId(i);
							break;
						}
					}

					// if flow created on main thread
					if(f->threadQueueId() == -1)
					{
						SLOG_INFO(<< "No queue in flow. Must be new")
						
						if(_currentQueue >= _workerThreads.size())
						{
							_currentQueue = 0;
						}
						f->threadQueueId(_currentQueue);
						_currentQueue++;
					}
				}

				return f;
			}
			else if ( (tmp->flowState() == Flow::State::FINISHED_NOTIFIED 
								|| tmp->flowState() == Flow::State::FINISHED) 
						&& !tmp->havePktsInQueue()
						&& ((packet->timestamp().tv_sec - tmp->lastPacketTimestamp().tv_sec ) 
									> CommonConfig::instance()->tcpFlowCloseWaitTimeout()))
			{
				SLOG_INFO(<< "Flow [" << *tmp << "] found but finished. Removing from list and adding new one")
				notifyFlowFinished(tmp);
				_flows.erase(it);
				std::shared_ptr<Flow>  f(new Flow(packet, _protocolModelDb));
				_flows.insert(std::make_pair(f->flowHash(), f));

				if( CommonConfig::instance()->workerThreadsPerQueue() >0 && _workerThreads.size() > 0)
				{
					// Will tell if flow is on thread
					for(int i = 0; i < _workerThreads.size(); i++)
					{
						if(_workerThreads[i].get_id() == std::this_thread::get_id() )
						{
							f->threadQueueId(i);
							break;
						}
					}

					// if flow created on main thread
					if(f->threadQueueId() == -1)
					{
						SLOG_INFO(<< "No queue in flow. Must be new")
						
						if(_currentQueue >= _workerThreads.size())
						{
							_currentQueue = 0;
						}
						f->threadQueueId(_currentQueue);
						_currentQueue++;
					}
				}

				return f;
			}
		}



		return it->second;

	}
}

std::shared_ptr<Flow> FlowManager::getFlow(uint32_t hash)
{
	std::lock_guard<std::mutex> guard(_flowsMutex);
	auto it = _flows.find(hash);

	if(it != _flows.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}


void FlowManager::deleteFlow(IPv4Packet* packet)
{
	std::lock_guard<std::mutex> guard(_flowsMutex);
	size_t num = _flows.erase(packet->flowHash());

	SLOG_INFO(<< num << " flows deleted");
}

void FlowManager::deleteFlow(std::shared_ptr<Flow> flow)
{
	std::lock_guard<std::mutex> guard(_flowsMutex);
	if(flow->havePktsInQueue())
	{
		// if we're here it's safe to assume we've tried to notify
		flow->setFinishedAndNotified();
		SLOG_INFO(<< "Not deleting because still packets to process")
		return;
	}

	size_t num = _flows.erase(flow->flowHash());

	SLOG_INFO(<< num << " flows deleted");
}

void FlowManager::deleteFlow(uint32_t hash)
{
	std::lock_guard<std::mutex> guard(_flowsMutex);
	size_t num = _flows.erase(hash);

	SLOG_INFO(<< num << " flows deleted");
}

void FlowManager::addFlowFinishedObserver(FlowFinishedObserver* observer)
{
	_flow_finished_observers.push_back(observer);
}

void FlowManager::notifyFlowFinished(std::shared_ptr<Flow> flow)
{
	if(flow->flowState() == Flow::State::FINISHED_NOTIFIED)
	{
		SLOG_INFO(<< "Not notifying because in FINISHED_NOTIFIED")
		return;
	}

	if(flow->fiveTuple().transport == IPPROTO_TCP)
	{
		// If there are only 3 packet overall don't notify because it may just
		// be a TCP with a second FIN
		if(flow->pktCount() == 0 || flow->overallPktCount() < 3)
		{
			SLOG_INFO(<< "Not notifying about finished becasue no packets");
			flow->setFinishedAndNotified();
			return;
		}

		/*if ( flow->havePktsInQueue() && !_finishing )
		{
			SLOG_INFO(<< "Not notifying because packets in flow")
			return;
		}*/
	}
	else
	{
		if(flow->pktCount() == 0 || flow->overallPktCount() < 2)
		{
			SLOG_INFO(<< "Not notifying about finished becasue too few packets");
			flow->setFinishedAndNotified();
			return;
		}
	}

	for(auto it = _flow_finished_observers.begin(); it != _flow_finished_observers.end(); ++it)
	{
		SLOG_INFO(<< "Notifing observer about Flow finished");
		(*it)->flowFinished(flow);
	}
	flow->setFinishedAndNotified();
}

void FlowManager::addFlowClassifiedObserver(FlowClassifiedObserver* observer)
{
	_flow_classified_observers.push_back(observer);
}

void FlowManager::notifyFlowClassified(std::shared_ptr<Flow> flow)
{
	for(auto it = _flow_classified_observers.begin(); it != _flow_classified_observers.end(); ++it)
	{
		(*it)->flowClassified(flow);
	}
}

void FlowManager::finished()
{
	
	// Wait for all queues to be empty before deleting flows form the manager
	for( int i = 0; i < _threadQueues.size(); i++ )
	{
		while( !_threadQueues[i]->empty() )
		{
			std::this_thread::sleep_for( std::chrono::microseconds(10) );
		}
	}

	_finishing = true;
	std::lock_guard<std::mutex> guard(_flowsMutex);
	for(auto it = _flows.begin(); it != _flows.end(); )
	{
		auto flow = it->second;
		while(flow->havePktsInQueue())
		{
			std::this_thread::sleep_for( std::chrono::microseconds(10) );
		}

		
		auto origIt = it;
		++it;

		notifyFlowFinished(flow);
		_flows.erase(origIt);
	}

	_finishing = false;
}