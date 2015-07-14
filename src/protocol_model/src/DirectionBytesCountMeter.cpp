
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
	AttributeMeter(2),
	_overall_byte_size(0)
{

}


void DirectionBytesCountMeter::calculateMeasurement(Flow* flow, 
									IPv4Packet* currentPacket )
{
	if(currentPacket->dataSize() <= 0)
        return;

    uint64_t OtoD_size = _fingerprint[0] * _overall_byte_size;
	uint64_t DtoO_size = _fingerprint[1] * _overall_byte_size;

	if( flow->currentPacketDirection() == Flow::Direction::ORIG_TO_DEST )
	{
		OtoD_size += currentPacket->dataSize();
	}
	else if( flow->currentPacketDirection() == Flow::Direction::DEST_TO_ORIG )
	{
		DtoO_size += currentPacket->dataSize();
	}
	else
	{
		// Shouldn't happen but just in case
		return;
	}

	_overall_byte_size += currentPacket->dataSize();

	_fingerprint[0] = OtoD_size / _overall_byte_size;
	_fingerprint[1] = DtoO_size / _overall_byte_size;
}