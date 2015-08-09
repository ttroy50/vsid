
#include "ByteFrequencyFirstOrigToDestPacket.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;
std::unique_ptr<AttributeMeter> create_first_packet_byte_meter()
{
	std::unique_ptr<AttributeMeter> tmp(new ByteFrequencyFirstOrigToDestPacket());
	return tmp;
}

Vsid::Registrar ByteFrequencyFirstOrigToDestPacket::registrar("ByteFrequencyFirstOrigToDestPacket", &create_first_packet_byte_meter);

ByteFrequencyFirstOrigToDestPacket::ByteFrequencyFirstOrigToDestPacket() :
	AttributeMeter(256),
	_overall_byte_size(0)
{

}


double ByteFrequencyFirstOrigToDestPacket::at(size_t pos)
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

void ByteFrequencyFirstOrigToDestPacket::calculateMeasurement(Flow* flow, 
									IPv4Packet* currentPacket )
{
	if( !flow->isFirstPacket() )
		return;

	if( currentPacket->dataSize() <= 0 )
		return;

	const u_char* data = currentPacket->data();
	for(size_t i = 0; i < currentPacket->dataSize(); i++ )
	{
		_fingerprint[*data]++;
		data++;
	}
	_overall_byte_size += currentPacket->dataSize();

}