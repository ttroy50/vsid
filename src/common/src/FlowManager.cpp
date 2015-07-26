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
	_shutdown(false)
{
	
}

void FlowManager::init()
{
	for( int i = 0; i < CommonConfig::instance()->workerThreadsPerQueue(); i++ )
	{
		_threadQueues.push_back(new boost::lockfree::queue<IPv4Packet*>(1024));
		_workerThreads.push_back( std::thread(&FlowManager::processPackets, this, i) );
		
	}
}

FlowManager::~FlowManager()
{
	_shutdown = true;
	for( int i = 0; i < _workerThreads.size(); i++ )
	{
		_workerThreads[i].join();
		IPv4Packet* packet = NULL;
		while( _threadQueues[i]->pop(packet) )
		{
			if(packet != NULL)
			{
				delete packet;
			}
		}

		boost::lockfree::queue<IPv4Packet*> * tmp = _threadQueues[i];
		delete tmp;

	}
}

void FlowManager::processPackets(int queue_id)
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
			std::this_thread::sleep_for( std::chrono::milliseconds(10) );
		}
	}
}

void FlowManager::processPacket(IPv4Packet* packet)
{
	try
	{
		std::shared_ptr<Flow> flow = getFlow(packet);

		if( flow )
		{
			bool classified = flow->flowClassified();

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
		}
		else
		{
			SLOG_ERROR(<< "Unable to find flow");
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
	std::shared_ptr<Flow> flow = getFlow(packet);

	if( flow )
	{
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
				return flow;
			}
			else
			{
				SLOG_ERROR(<< "Unable to push packet onto queue");
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
		return f;
	}
	else
	{
		if( packet->protocol() == IPPROTO_UDP )
		{
			std::shared_ptr<Flow> tmp = it->second;
			
			if( (packet->timestamp().tv_sec - tmp->lastPacketTimestamp().tv_sec ) > CommonConfig::instance()->udpFlowTimeout())
			{
				SLOG_INFO(<< "Flow [" << *tmp << "] found but finished. Removing from list and adding new one")
				notifyFlowFinished(tmp);
				_flows.erase(it);
				std::shared_ptr<Flow>  f(new Flow(packet, _protocolModelDb));
				_flows.insert(std::make_pair(f->flowHash(), f));
				return f;
			}
		}
		else if( packet->protocol() == IPPROTO_TCP )
		{
			// TODO Check for RST or FIN flags
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
	if(flow->fiveTuple().transport == IPPROTO_TCP)
	{
		// If there is only 1 packet don't notify because it may just
		// be a TCP with a second FIN
		// TODO update to TCP only
		if(flow->pktCount() < 3)
		{
			SLOG_INFO(<< "Not notifying about finished becasue only 1 packet");
			return;
		}
	}
	for(auto it = _flow_finished_observers.begin(); it != _flow_finished_observers.end(); ++it)
	{
		SLOG_INFO(<< "Notifing observer about Flow finished");
		(*it)->flowFinished(flow);
	}
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
			std::this_thread::sleep_for( std::chrono::milliseconds(10) );
		}
	}

	std::lock_guard<std::mutex> guard(_flowsMutex);
	for(auto it = _flows.begin(); it != _flows.end(); )
	{
		auto flow = it->second;
		auto origIt = it;
		++it;

		notifyFlowFinished(flow);
		_flows.erase(origIt);
	}
}