
#include "ByteFrequencyFirstDestToOrigPacket.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_first_packet_dest_orig_byte_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new ByteFrequencyFirstDestToOrigPacket());
    return tmp;
}

Vsid::Registrar ByteFrequencyFirstDestToOrigPacket::registrar("ByteFrequencyFirstDestToOrigPacket", &create_first_packet_dest_orig_byte_meter);

ByteFrequencyFirstDestToOrigPacket::ByteFrequencyFirstDestToOrigPacket() :
    AttributeMeter(256),
    _done(false),
    _overall_byte_size(0)
{

}

double ByteFrequencyFirstDestToOrigPacket::at(size_t pos)
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


void ByteFrequencyFirstDestToOrigPacket::calculateMeasurement(Flow* flow, 
                                    IPv4Packet* currentPacket )
{
    if( _done || currentPacket->dataSize() <= 0 )
    return;

    std::vector<int> count(_fingerprint_size, 0);

    const u_char* data = currentPacket->data();
    for(size_t i = 0; i < currentPacket->dataSize(); i++ )
    {
        _fingerprint[*data]++;
        data++;
    }

    _overall_byte_size += currentPacket->dataSize();

    _done = true;
}