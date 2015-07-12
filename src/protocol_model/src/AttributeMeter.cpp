
#include "AttributeMeter.h"
#include "Logger.h"

using namespace std;
using namespace Vsid;

AttributeMeter::AttributeMeter(size_t fingerprint_size) :
	_fingerprint_size(fingerprint_size),
	_fingerprint(fingerprint_size, 0.0),
	_enabled(false)
{

}

double AttributeMeter::front()
{
	return at(0);
}


double AttributeMeter::at(size_t pos)
{
	if(pos < _fingerprint.size())
	{
		return _fingerprint[pos];
	}
	else
	{
		//TODO return or throw??
		return -1;
	}
}