
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
	AttributeMeter(255),
	_overall_byte_size(0)
{
}

void ByteFrequencyMeter::calculateMeasurement(Flow* flow, 
													IPv4Packet* currentPacket )
{	
	if(currentPacket->dataSize() <= 0)
		return;


	// TODO Limit how many packets this can run on
		
		 
	//std::vector<double> results(_fingerprint_size, 0.0);
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
		// to just get for this packet
		//double result = (double)count[i] / currentPacket->dataSize();
		
		// to get for all packets so far
		_fingerprint[i] = (double)((_fingerprint[i] * ( _overall_byte_size - currentPacket->dataSize() )) 
								+ count[i]) / _overall_byte_size;
		
	}
}