#include "AttributeMeterFactory.h"
#include "Logger.h"

using namespace Vsid;

std::unique_ptr<AttributeMeterFactory> AttributeMeterFactory::_instance;
std::once_flag AttributeMeterFactory::_onceFlag;

AttributeMeterFactory* AttributeMeterFactory::instance()
{
	try
	{
		std::call_once(_onceFlag,
	        [] {
	    if(!_instance) 
	            _instance.reset(new AttributeMeterFactory);
	    	}
	    );
	}
	catch(std::runtime_error& e)
	{
		// not thread safe but this should be initialised alread in main thread
		// Not sure why I was sometimes seeing exception from call_once. Need time to investigate
		if(!_instance)
			_instance.reset(new AttributeMeterFactory);
	}

    return _instance.get();
}

std::unique_ptr<AttributeMeter> AttributeMeterFactory::create(const std::string& name)
{
	auto it = _factories.find(name);
	if(it != _factories.end())
	{
		return it->second();
	}

	SLOG_ERROR(<< "Unable to create class [" << name << "]");
	return nullptr;
}

void AttributeMeterFactory::registerFactory(const std::string& name, CreateFn fn)
{
	std::cout << "register " << name << std::endl;
	SLOG_INFO(<< "Registering class [" << name << "]");
	_factories[name] = fn;

	// Default to false and only enable after it is read from the ProtocolDb
	_enabledAttributes[name] = false;
}


std::vector<std::shared_ptr<AttributeMeter> > AttributeMeterFactory::getAllMeters()
{
	SLOG_INFO(<< "getting meters");
	std::vector<std::shared_ptr<AttributeMeter> > ret;

	for( auto it = _enabledAttributes.begin(); it != _enabledAttributes.end(); ++it)
	{		
		if(it->second)
		{
			auto attr = create(it->first);
			
			if( !attr)
			{
				SLOG_ERROR( << "Error creating attribute for " << it->first)
				continue;
			}

			SLOG_INFO(<< "getting meter " << it->first);
			std::shared_ptr<AttributeMeter> tmp { std::move(attr) };
			ret.push_back(tmp);
		}
		else
		{
			SLOG_INFO( << "Attribute Disabled " << it->first)
		}
	}

	return ret;
}

void AttributeMeterFactory::enableAttribute(const std::string& name)
{
	auto it = _enabledAttributes.find(name);

	if( it == _enabledAttributes.end())
	{
		SLOG_ERROR(<< "Unable to find attribute : " << name);
		return;
	}

	SLOG_INFO( << "enabling attribute : " << name);

	it->second = true;
}