#include "FlowManager.h"
#include "Flow.h"
#include "Logger.h"
#include "Hasher.h"
#include "IPv4.h"

using namespace std;
using namespace VSID_TRAINING;

FlowManager::FlowManager()
{
}

Flow* FlowManager::addPacket(IPv4* packet)
{
	Flow* flow = getFlow(packet);
	if(flow != NULL)
		flow->addPacket(packet);

	return flow;
}

bool FlowManager::flowExists(IPv4* packet)
{
	return false;
}

bool FlowManager::flowExists(uint32_t hash)
{
	return false;
}

Flow* FlowManager::getFlow(IPv4* packet)
{
	Flow* flow = NULL;


	return flow;
}

Flow* FlowManager::getFlow(uint32_t hash)
{
	Flow* flow = NULL;

	return flow;
}