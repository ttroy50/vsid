
#include "ByteFrequency.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_byte_frequency_meter()
{
	std::unique_ptr<AttributeMeter> tmp(new ByteFrequencyMeter());
	return tmp;
}

Vsid::Registrar ByteFrequencyMeter::registrar("ByteFrequencyMeter", &create_byte_frequency_meter);

ByteFrequencyMeter::ByteFrequencyMeter() :
	AttributeMeter(256),
	_overall_byte_size(0)
{
}

double ByteFrequencyMeter::at(size_t pos)
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

void ByteFrequencyMeter::calculateMeasurement(Flow* flow, 
													IPv4Packet* currentPacket )
{	
	if(currentPacket->dataSize() <= 0)
		return;

	const u_char* data = currentPacket->data();
	for(size_t i = 0; i < currentPacket->dataSize(); i++ )
	{
		_fingerprint[*data]++;
		data++;
	}

	_overall_byte_size += currentPacket->dataSize();

}