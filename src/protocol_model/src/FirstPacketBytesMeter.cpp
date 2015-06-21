
#include "FirstPacketBytesMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;

std::unique_ptr<AttributeMeter> create_first_packet_byte_meter()
{
	std::unique_ptr<AttributeMeter> tmp(new FirstPacketBytesMeter());
	return tmp;
}

Vsid::Registrar FirstPacketBytesMeter::registrar("FirstPacketBytesMeter", &create_first_packet_byte_meter);

FirstPacketBytesMeter::FirstPacketBytesMeter() :
	AttributeMeter(255)
{

}

