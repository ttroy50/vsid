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
	_protocolModelDb(database)
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
	auto it = _flows.find(packet->flowHash());

	if(it == _flows.end())
	{
		return false;
	}

	return true;
}

bool FlowManager::flowExists(uint32_t hash)
{
	auto it = _flows.find(hash);

	if(it == _flows.end())
	{
		return false;
	}

	return true;
}

std::shared_ptr<Flow>  FlowManager::getFlow(IPv4Packet* packet)
{
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
	size_t num = _flows.erase(packet->flowHash());

	SLOG_INFO(<< num << " flows deleted");
}

void FlowManager::deleteFlow(std::shared_ptr<Flow> flow)
{
	size_t num = _flows.erase(flow->flowHash());

	SLOG_INFO(<< num << " flows deleted");
}

void FlowManager::deleteFlow(uint32_t hash)
{
	size_t num = _flows.erase(hash);

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
		auto flow = it->second;
		++it;

		notifyFlowFinished(flow);
		_flows.erase(it);
	}
}