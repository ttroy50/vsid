
#include "AttributeMeter.h"
#include "Logger.h"

using namespace std;
using namespace Vsid;

AttributeMeter::AttributeMeter(size_t fingerprint_size) :
	_fingerprint_size(fingerprint_size)
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