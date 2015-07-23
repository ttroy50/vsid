
#include "DataDirectionChangesFirstEightPacketsMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_data_direction_changes_first_8_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new DataDirectionChangesFirstEightPacketsMeter());
    return tmp;
}

Vsid::Registrar DataDirectionChangesFirstEightPacketsMeter::registrar("DataDirectionChangesFirstEightPacketsMeter", &create_data_direction_changes_first_8_meter);

DataDirectionChangesFirstEightPacketsMeter::DataDirectionChangesFirstEightPacketsMeter() :
    AttributeMeter(1)
{
}

void DataDirectionChangesFirstEightPacketsMeter::calculateMeasurement(Flow* flow, 
                                                    IPv4Packet* currentPacket )
{   
    if(flow->pktCount() > 8)
        return;

    if(currentPacket->dataSize() <= 0)
        return;
    
    uint64_t count = _fingerprint[0] * (flow->pktCount() - 1);

    if( flow->lastPacketDirection() != flow->currentPacketDirection() )
    {
        count++;
    }

    _fingerprint[0] = (double)count / flow->pktCount();
}