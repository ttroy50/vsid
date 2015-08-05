
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

        /*double prev = _overall_reaction_num * 115;
        _overall_reaction_num++;
        for(int i = 0; i < _fingerprint_size; i++)
        {
            if( i == mixed)
            {
                _fingerprint[i] = (double)((_fingerprint[i] *  prev )
                                     + mixed) / ( prev + 115);
            }
            else
            {
                _fingerprint[i] = (double)(_fingerprint[i] * prev ) 
                            /  (prev + 115);
            }
        }*/

        _overall_reaction_num++;

        for(int i = 0; i < _fingerprint_size; i++)
        {
            if( i == mixed )
            {
                _fingerprint[i] = (double)((_fingerprint[i] *  (_overall_reaction_num-1.0) )
                                     + 1.0) / _overall_reaction_num;
            }
            else
            {
                _fingerprint[i] = (double)((_fingerprint[i] *  (_overall_reaction_num-1.0) )
                                        ) / _overall_reaction_num;
            }
        }
    }

    memcpy(_lastPacket, currentData, NUM_AR_BYTES);
    _lastPacketSize = currentDataSize;
}