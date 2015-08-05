
#include "PayloadSizeFirstPacketMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"
#include <netinet/udp.h>
#include <netinet/tcp.h>

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_first_payload_size_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new PayloadSizeFirstPacketMeter());
    return tmp;
}

Vsid::Registrar PayloadSizeFirstPacketMeter::registrar("PayloadSizeFirstPacketMeter", &create_first_payload_size_meter);

PayloadSizeFirstPacketMeter::PayloadSizeFirstPacketMeter() :
    AttributeMeter(2)
{
    _fingerprint[1] = 1;
}

void PayloadSizeFirstPacketMeter::calculateMeasurement(Flow* flow, 
                                                    IPv4Packet* currentPacket )
{   
    if( !flow->isFirstPacket() )
        return;

    int size = 0;
    int max_size = 0;

    if (currentPacket->protocol() == IPPROTO_TCP) 
    {
        size = currentPacket->dataSize();
        max_size = 1460; // Normal MTU - TCP header size
    }
    else 
    {
        size = currentPacket->dataSize();
        max_size = 1476; // Normal MTU - UDP header size
    }

    // Need the probabilities to add up to 1
    _fingerprint[0] = (double)size / max_size;
    _fingerprint[1] = 1 - _fingerprint[0];
}