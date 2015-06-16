
#include "ProtocolModel.h"
#include "AttributeMeter.h"
#include "Logger.h"

using namespace std;
using namespace Vsid;

ProtocolModel::ProtocolModel()
{

}


std::shared_ptr<AttributeMeter> ProtocolModel::find(const std::string& name)
{
	for(AttributeMeterVector::iterator it = _attributeMeters.begin();
			it != _attributeMeters.end(); ++it)
	{
		if((*it)->name() == name)
			return (*it);
	}

	return std::shared_ptr<AttributeMeter>();
}


std::shared_ptr<AttributeMeter> ProtocolModel::front()
{
	return at(0);
}


std::shared_ptr<AttributeMeter> ProtocolModel::at(size_t pos)
{
	if(pos < _attributeMeters.size())
	{
		return _attributeMeters[pos];
	}
	else
	{
		return std::shared_ptr<AttributeMeter>();
	}
}