
#include "DirectionChangesFirst8PacketsMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_direction_changes_first_8_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new DirectionChangesFirst8PacketsMeter());
    return tmp;
}

Vsid::Registrar DirectionChangesFirst8PacketsMeter::registrar("DirectionChangesFirst8PacketsMeter", &create_direction_changes_first_8_meter);

DirectionChangesFirst8PacketsMeter::DirectionChangesFirst8PacketsMeter() :
    AttributeMeter(1)
{
}

void DirectionChangesFirst8PacketsMeter::calculateMeasurement(Flow* flow, 
                                                    IPv4Packet* currentPacket )
{   
    if(flow->pktCount() > 8)
        return;
    
    uint64_t count = _fingerprint[0] * (flow->pktCount() - 1);

    if( flow->lastPacketDirection() != flow->currentPacketDirection() )
    {
        count++;
    }

    _fingerprint[0] = (double)count / flow->pktCount();
}