
#include "ByteFrequencyFirst8PacketsMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_byte_frequency_first_8_packets_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new ByteFrequencyFirst8PacketsMeter());
    return tmp;
}

Vsid::Registrar ByteFrequencyFirst8PacketsMeter::registrar("ByteFrequencyFirst8PacketsMeter", &create_byte_frequency_first_8_packets_meter);

ByteFrequencyFirst8PacketsMeter::ByteFrequencyFirst8PacketsMeter() :
    AttributeMeter(256),
    _overall_byte_size(0)
{
}

double ByteFrequencyFirst8PacketsMeter::at(size_t pos)
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
            if(_overall_byte_size == 0)
                return 0;
            return (double)_fingerprint[pos] / _overall_byte_size;
        }
        else
        {
            //TODO return or throw??
            return -1;
        }
    }
}

void ByteFrequencyFirst8PacketsMeter::calculateMeasurement(Flow* flow, 
                                                    IPv4Packet* currentPacket )
{

    if(flow->pktCount() > 8)
        return;

    if(currentPacket->dataSize() <= 0)
        return;

    const u_char* data = currentPacket->data();
    for(size_t i = 0; i < currentPacket->dataSize(); i++ )
    {
        _fingerprint[*data]++;
        data++;
    }

    _overall_byte_size = currentPacket->dataSize();
}