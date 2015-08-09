
#include "FirstBitPositionsMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_bit_position_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new FirstBitPositionsMeter());
    return tmp;
}

Vsid::Registrar FirstBitPositionsMeter::registrar("FirstBitPositionsMeter", &create_bit_position_meter);

FirstBitPositionsMeter::FirstBitPositionsMeter() :
    AttributeMeter(64),
    _overall_bit_size(0)
{
}

double FirstBitPositionsMeter::at(size_t pos)
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
            if(_overall_bit_size == 0)
                return 0;

            return (double)_fingerprint[pos] / _overall_bit_size;
        }
        else
        {
            //TODO return or throw??
            return -1;
        }
    }
}


void FirstBitPositionsMeter::calculateMeasurement(Flow* flow, 
                                                    IPv4Packet* currentPacket )
{   
    if(currentPacket->dataSize() <= 0)
        return;

    int oneOffset = 0;
    int zeroOffset = 8;

    const u_char* payload = currentPacket->data();
    for(int i = 0; i < currentPacket->dataSize() && i < 4 ; i++)
    {
        int posOffset = i*16;
        oneOffset= 0;
        zeroOffset = 8;
        for( uint32_t mask = 0x80; mask != 0; mask >>= 1)
        {
            if (payload[i] & mask)
            {
                _fingerprint[posOffset + oneOffset]++;
            }
            else
            {
                _fingerprint[posOffset + zeroOffset]++;
            }
            oneOffset++;
            zeroOffset++;
        }
    }

    _overall_bit_size += 32;
}