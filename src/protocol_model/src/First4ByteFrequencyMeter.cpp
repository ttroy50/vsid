
#include "First4ByteFrequencyMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_byte_first_four_frequency_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new First4ByteFrequencyMeter());
    return tmp;
}

Vsid::Registrar First4ByteFrequencyMeter::registrar("First4ByteFrequencyMeter", &create_byte_first_four_frequency_meter);

First4ByteFrequencyMeter::First4ByteFrequencyMeter() :
    AttributeMeter(256),
    _overall_byte_size(0)
{
}

void First4ByteFrequencyMeter::calculateMeasurement(Flow* flow, 
                                                    IPv4Packet* currentPacket )
{   
    if(currentPacket->dataSize() <= 0)
        return;


    // TODO Limit how many packets this can run on
        
    std::vector<int> count(_fingerprint_size, 0);

    const u_char* data = currentPacket->data();
    for(size_t i = 0; i < currentPacket->dataSize() || i < 4 ; i++ )
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