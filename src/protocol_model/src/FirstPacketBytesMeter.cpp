
#include "FirstPacketBytesMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;
std::unique_ptr<AttributeMeter> create_first_packet_byte_meter()
{
	std::unique_ptr<AttributeMeter> tmp(new FirstPacketBytesMeter());
	return tmp;
}

Vsid::Registrar FirstPacketBytesMeter::registrar("FirstPacketBytesMeter", &create_first_packet_byte_meter);

FirstPacketBytesMeter::FirstPacketBytesMeter() :
	AttributeMeter(256)
{

}

void FirstPacketBytesMeter::calculateMeasurement(Flow* flow, 
									IPv4Packet* currentPacket )
{
	SLOG_INFO(<< "FirstPacket " << flow->isFirstPacket() << " size " << currentPacket->dataSize());
	if( !flow->isFirstPacket() )
		return;

	if( currentPacket->dataSize() <= 0 )
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
}