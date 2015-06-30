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
			if( (packet->timestamp().tv_sec - (*(it))->lastPacketTimestamp().tv_sec ) > 120)
			{
				SLOG_INFO(<< "Flow [" << *f << "] found but finished. Removing from list and adding new one")
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

