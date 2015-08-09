
#include "ActionReactionFirst3ByteHashMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> action_reaction_first_3_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new ActionReactionFirst3ByteHashMeter());
    return tmp;
}

Vsid::Registrar ActionReactionFirst3ByteHashMeter::registrar("ActionReactionFirst3ByteHashMeter", &action_reaction_first_3_meter);

ActionReactionFirst3ByteHashMeter::ActionReactionFirst3ByteHashMeter() :
    AttributeMeter(115),
    _lastPacketSize(0),
    _overall_reaction_num(0)
{
}


double ActionReactionFirst3ByteHashMeter::at(size_t pos)
{
    // If the neter comes from the DB
    if (_fromDb)
    {
        return AttributeMeter::at(pos);
    }
    else
    {
        if(pos < _fingerprint.size())
        {
            if(_overall_reaction_num == 0)
                return 0;
            return (double)_fingerprint[pos] / _overall_reaction_num;
        }
        else
        {
            //TODO return or throw??
            return -1;
        }
    }
}


void ActionReactionFirst3ByteHashMeter::calculateMeasurement(Flow* flow, 
                                                    IPv4Packet* currentPacket )
{   
    if( currentPacket->dataSize() <= 0)
        return;

    u_char currentData[NUM_AR_BYTES];
    size_t currentDataSize = 0;
    for(size_t i = 0; i < NUM_AR_BYTES; i++)
    {
        if(currentPacket->dataSize() > i )
        {
            currentData[i] = *(currentPacket->data()+i);
            currentDataSize++;
        }
        else
        {
            currentData[i] = '\0';
        }
    }

    if( !flow->isFirstPacket() && 
            flow->lastPacketDirection() != flow->currentPacketDirection() )
    {
        uint8_t crosssum_one = 0;
        uint8_t crosssum_two = 0;
        for ( size_t i = 0; i < NUM_AR_BYTES; i++ )
        {
            uint8_t byte_one =
                    static_cast<uint8_t>(_lastPacket[i]);
            uint8_t byte_two =
                static_cast<uint8_t>(currentData[i]);
            uint8_t rest = 0;
            do {
                rest = byte_one%10;
                byte_one = byte_one/10;
                crosssum_one += rest;
            } while (byte_one > 0);

            rest = 0;
            do {
                rest = byte_two%10;
                byte_two = byte_two/10;
                crosssum_two += rest;
            } while (byte_two > 0);
            
        }

        uint8_t mixed = crosssum_one + crosssum_two;

        _fingerprint[mixed]++;
        _overall_reaction_num++;
    }

    memcpy(_lastPacket, currentData, NUM_AR_BYTES);
    _lastPacketSize = currentDataSize;
}