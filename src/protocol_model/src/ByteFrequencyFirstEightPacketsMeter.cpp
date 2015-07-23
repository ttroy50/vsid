
#include "ByteFrequencyFirstEightPacketsMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_byte_frequency_first_8_packets_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new ByteFrequencyFirstEightPacketsMeter());
    return tmp;
}

Vsid::Registrar ByteFrequencyFirstEightPacketsMeter::registrar("ByteFrequencyFirstEightPacketsMeter", &create_byte_frequency_first_8_packets_meter);

ByteFrequencyFirstEightPacketsMeter::ByteFrequencyFirstEightPacketsMeter() :
    AttributeMeter(256),
    _overall_byte_size(0)
{
}

void ByteFrequencyFirstEightPacketsMeter::calculateMeasurement(Flow* flow, 
                                                    IPv4Packet* currentPacket )
{

    if(flow->pktCount() > 8)
        return;

    if(currentPacket->dataSize() <= 0)
        return;

           
    std::vector<int> count(_fingerprint_size, 0);

    const u_char* data = currentPacket->data();
    for(size_t i = 0; i < currentPacket->dataSize(); i++ )
    {
        count[*data]++;
        data++;
    }

    _overall_byte_size = currentPacket->dataSize();

    for(size_t i = 0; i <_fingerprint_size; i++ )
    {
        _fingerprint[i] = (double)((_fingerprint[i] * ( _overall_byte_size - currentPacket->dataSize() )) 
                                + count[i]) / _overall_byte_size;
    }
}