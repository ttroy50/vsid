
#include "FirstFourByteFrequencyMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_byte_first_four_frequency_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new FirstFourByteFrequencyMeter());
    return tmp;
}

Vsid::Registrar FirstFourByteFrequencyMeter::registrar("FirstFourByteFrequencyMeter", &create_byte_first_four_frequency_meter);

FirstFourByteFrequencyMeter::FirstFourByteFrequencyMeter() :
    AttributeMeter(256),
    _overall_byte_size(0)
{
}

void FirstFourByteFrequencyMeter::calculateMeasurement(Flow* flow, 
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