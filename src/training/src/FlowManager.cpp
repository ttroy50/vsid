#include "FlowManager.h"
#include "Flow.h"
#include "Logger.h"
#include "Hasher.h"
#include "IPv4.h"

using namespace std;
using namespace VSID_TRAINING;

FlowManager* FlowManager::_instance = NULL;

FlowManager* FlowManager::getInstance()
{
	if(_instance == NULL)
	{
		_instance = new FlowManager();
	}
	return _instance;
}

FlowManager::FlowManager()
{
}

FlowManager::~FlowManager()
{

}

std::shared_ptr<Flow> FlowManager::addPacket(IPv4Packet* packet)
{
	std::shared_ptr<Flow> flow = getFlow(packet);
	if(flow != NULL)
	{
		flow->addPacket(packet);
		SLOG_INFO(<< "Packet added to flow : " << *flow);	
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
		return NULL;
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

