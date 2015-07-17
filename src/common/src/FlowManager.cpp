#include "FlowManager.h"
#include "Flow.h"
#include "Logger.h"
#include "Hasher.h"
#include "IPv4.h"

using namespace std;
using namespace VsidCommon;


FlowManager::FlowManager()
{
}

FlowManager::~FlowManager()
{

}

std::shared_ptr<Flow> FlowManager::addPacket(IPv4Packet* packet)
{
	std::shared_ptr<Flow> flow = getFlow(packet);

	if( flow )
	{
		flow->addPacket(packet);
		SLOG_INFO(<< "Packet added to flow : " << *flow);	

		if(flow->flowState() == Flow::State::FINISHED)
		{
			notifyFlowFinished(flow);
			deleteFlow(flow);
			SLOG_INFO(<< "Flow in finished state. Removing")
			return nullptr;
		}
	}

	return flow;
}

bool FlowManager::flowExists(IPv4Packet* packet)
{
	// temp created to lookup flow
	std::shared_ptr<Flow> f (new Flow(packet));
	FlowSet::iterator it = _flows.find(f);

	if(it == _flows.end())
	{
		return false;
	}

	return true;
}

bool FlowManager::flowExists(uint32_t hash)
{
	// temp created to lookup flow
	std::shared_ptr<Flow> f (new Flow(hash));
	FlowSet::iterator it = _flows.find(f);

	if(it == _flows.end())
	{
		return false;
	}

	return true;
}

std::shared_ptr<Flow>  FlowManager::getFlow(IPv4Packet* packet)
{
	SLOG_INFO(<< _flows.size() << " flows in manager");

	// temp created to lookup flow
	std::shared_ptr<Flow>  f(new Flow(packet));
	FlowSet::iterator it = _flows.find(f);

	if(it == _flows.end())
	{
		_flows.insert(f);
		SLOG_INFO(<< "New Flow added : " << *f);
		return f;
	}
	else
	{
		if( packet->protocol() == IPPROTO_UDP )
		{
			std::shared_ptr<Flow> tmp = *(it);
			// TODO pass in as config
			if( (packet->timestamp().tv_sec - tmp->lastPacketTimestamp().tv_sec ) > 120)
			{
				SLOG_INFO(<< "Flow [" << *f << "] found but finished. Removing from list and adding new one")
				notifyFlowFinished(tmp);
				_flows.erase(it);
				_flows.insert(f);
				return f;
			}
		}
		else if( packet->protocol() == IPPROTO_TCP )
		{
			// TODO Check for RST or FIN flags
		}



		return *(it);

	}
}

std::shared_ptr<Flow> FlowManager::getFlow(uint32_t hash)
{
	std::shared_ptr<Flow> f (new Flow(hash));
	FlowSet::iterator it = _flows.find(f);

	if(it != _flows.end())
	{
		return *(it);
	}
	else
	{
		return nullptr;
	}
}


void FlowManager::deleteFlow(IPv4Packet* packet)
{
	// temp created to lookup flow
	std::shared_ptr<Flow> f (new Flow(packet));
	size_t num = _flows.erase(f);

	SLOG_INFO(<< num << " flows deleted");
}

void FlowManager::deleteFlow(std::shared_ptr<Flow> flow)
{
	size_t num = _flows.erase(flow);

	SLOG_INFO(<< num << " flows deleted");
}

void FlowManager::deleteFlow(uint32_t hash)
{
	// temp created to lookup flow
	std::shared_ptr<Flow> f (new Flow(hash));
	size_t num = _flows.erase(f);

	SLOG_INFO(<< num << " flows deleted");
}

void FlowManager::addFlowFinishedObserver(FlowFinishedObserver* observer)
{
	_flow_finished_observers.push_back(observer);
}

void FlowManager::notifyFlowFinished(std::shared_ptr<Flow> flow)
{
	// If there is only 1 packet don't notify because it may just
	// be a TCP with a second FIN
	// TODO update to TCP only
	if(flow->pktCount() == 1)
	{
		SLOG_INFO(<< "Not notifying about finished becasue only 1 packet");
		return;
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
	for(auto it = _flows.begin(); it != _flows.end(); )
	{
		auto flow = (*it);
		++it;

		notifyFlowFinished(flow);
		_flows.erase(it);
	}
}