
#include "DirectionBytesCountMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;


static std::unique_ptr<AttributeMeter> create_direction_byte_count_meter()
{
	std::cout << "Creating byte" << std::endl;
	std::unique_ptr<AttributeMeter> tmp(new DirectionBytesCountMeter());
	return tmp;
}

Vsid::Registrar DirectionBytesCountMeter::registrar("DirectionBytesCountMeter", &create_direction_byte_count_meter);
//REGISTER_ATTRIBUTE_METER(DirectionBytesCountMeter, &create_direction_byte_count_meter);

DirectionBytesCountMeter::DirectionBytesCountMeter() :
	AttributeMeter(2)
{

}
