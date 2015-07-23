
#include "DirectionChangesMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_direction_changes_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new DirectionChangesMeter());
    return tmp;
}

Vsid::Registrar DirectionChangesMeter::registrar("DirectionChangesMeter", &create_direction_changes_meter);

DirectionChangesMeter::DirectionChangesMeter() :
    AttributeMeter(1)
{
}

void DirectionChangesMeter::calculateMeasurement(Flow* flow, 
                                                    IPv4Packet* currentPacket )
{   
    uint64_t count = _fingerprint[0] * (flow->pktCount() - 1);

    if( flow->lastPacketDirection() != flow->currentPacketDirection() )
    {
        count++;
    }

    _fingerprint[0] = (double)count / flow->pktCount();
}