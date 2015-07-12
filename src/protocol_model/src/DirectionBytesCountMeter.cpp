
#include "DirectionBytesCountMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

static std::unique_ptr<AttributeMeter> create_direction_byte_count_meter()
{
	std::unique_ptr<AttributeMeter> tmp(new DirectionBytesCountMeter());
	return tmp;
}

Vsid::Registrar DirectionBytesCountMeter::registrar("DirectionBytesCountMeter", &create_direction_byte_count_meter);

DirectionBytesCountMeter::DirectionBytesCountMeter() :
	AttributeMeter(2)
{

}


void DirectionBytesCountMeter::calculateMeasurement(Flow* flow, 
									IPv4Packet* currentPacket )
{
	SLOG_INFO(<< "Not implemented");
	//std::vector<int> count(_fingerprint_size, 0);


	/*if( !flow->packetDirection(currentPacket) == Flow::Direction::ORIG_TO_DEST )
	{

		return results;
	}

	u_char* data = currentPacket->data();
	for(size_t i = 0; i < currentPacket->dataSize(); i++ )
	{
		count[*data]++;
		data++;
	}

	for(size_t i = 0; i <_fingerprint_size; i++ )
	{
		results[i] = (double)count[i] / currentPacket->dataSize();
	}
	*/
	return;

}