
#include "ByteFrequency.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;

std::unique_ptr<AttributeMeter> create_byte_frequency()
{
	std::unique_ptr<AttributeMeter> tmp(new ByteFrequency());
	return tmp;
}

Vsid::Registrar ByteFrequency::registrar("ByteFrequency", &create_byte_frequency);

ByteFrequency::ByteFrequency()
{

}

