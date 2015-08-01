
#include "EntropyFirstOrigToDestPacket.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;
std::unique_ptr<AttributeMeter> create_first_packet_entropy_orig_dest_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new EntropyFirstOrigToDestPacket());
    return tmp;
}

Vsid::Registrar EntropyFirstOrigToDestPacket::registrar("EntropyFirstOrigToDestPacket", &create_first_packet_entropy_orig_dest_meter);

EntropyFirstOrigToDestPacket::EntropyFirstOrigToDestPacket() :
    AttributeMeter(1)
{

}

void EntropyFirstOrigToDestPacket::calculateMeasurement(Flow* flow, 
                                    IPv4Packet* currentPacket )
{
    if( !flow->isFirstPacket() )
        return;

    if( currentPacket->dataSize() <= 0 )
        return;

    std::vector<int> count(256, 0);

    const u_char* data = currentPacket->data();
    for(size_t i = 0; i < currentPacket->dataSize(); i++ )
    {
        count[*data]++;
        data++;
    }

    for(size_t i = 0; i < 256; i++ )
    {
        double probability = (double)count[i] / currentPacket->dataSize();
        if (probability > 0)
            _fingerprint[0] -= probability*log2(probability);
    }
    _fingerprint[0] = _fingerprint[0]/8;
}