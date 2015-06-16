
#include "AttributeMeter.h"
#include "Logger.h"

using namespace std;
using namespace Vsid;

AttributeMeter::AttributeMeter()
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