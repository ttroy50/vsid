
#include "AttributeMeter.h"
#include "Logger.h"

using namespace std;
using namespace Vsid;

AttributeMeter::AttributeMeter(size_t fingerprint_size) :
	_fingerprint_size(fingerprint_size),
	_fingerprint(fingerprint_size, 0.0),
	_enabled(false)
{

	// These are the vlues when used for an observation.
	// IF this is an attribute in the ProtocolModelDb, then these are overwritten during
	// reading of config
	// 1 is used because it assumes that an observation has a flowCount of 1
	_klFixMultiplicator = (double) 1.0 / (fingerprint_size + 1.0);
	_klFixIncrement = (1.0) / (fingerprint_size + 1.0);
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

void AttributeMeter::merge(std::shared_ptr<AttributeMeter> other)
{
	SLOG_INFO(<< "Merging meter " << other->name() << " into " << name());
	if(other->name() != this->name())
	{
		SLOG_ERROR(<< "meters don't match");
		return;
	}

	for(size_t i = 0; i < this->size(); i++)
	{
		SLOG_INFO(<< "[" << i << "] : this =" << this->at(i) << " other = " << other->at(i) << " flowCount = " << _flowCount);
		_fingerprint[i] = ((_fingerprint[i] * _flowCount) + other->at(i) ) / (_flowCount+1); 
	}
	SLOG_INFO(<< "Meters Merged :" << name())
	_flowCount++;
}