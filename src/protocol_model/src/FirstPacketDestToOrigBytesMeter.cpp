
#include "FirstPacketDestToOrigBytesMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_first_packet_dest_orig_byte_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new FirstPacketDestToOrigBytesMeter());
    return tmp;
}

Vsid::Registrar FirstPacketDestToOrigBytesMeter::registrar("FirstPacketDestToOrigBytesMeter", &create_first_packet_dest_orig_byte_meter);

FirstPacketDestToOrigBytesMeter::FirstPacketDestToOrigBytesMeter() :
    AttributeMeter(256),
    _done(false)
{

}

void FirstPacketDestToOrigBytesMeter::calculateMeasurement(Flow* flow, 
                                    IPv4Packet* currentPacket )
{
    if( _done || currentPacket->dataSize() <= 0 )
    return;

    std::vector<int> count(_fingerprint_size, 0);

    const u_char* data = currentPacket->data();
    for(size_t i = 0; i < currentPacket->dataSize(); i++ )
    {
        count[*data]++;
        data++;
    }

    for(size_t i = 0; i <_fingerprint_size; i++ )
    {
        _fingerprint[i] = (double)count[i] / currentPacket->dataSize();
    }

    _done = true;
}