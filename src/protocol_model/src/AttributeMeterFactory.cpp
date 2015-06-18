#include "AttributeMeterFactory.h"
#include "Logger.h"

using namespace Vsid;

std::unique_ptr<AttributeMeterFactory> AttributeMeterFactory::_instance;
std::once_flag AttributeMeterFactory::_onceFlag;

AttributeMeterFactory* AttributeMeterFactory::instance()
{
	std::cout << "accessing amf" << std::endl;
	//std::call_once(_onceFlag,
    //    [] {
    if(!_instance) 
            _instance.reset(new AttributeMeterFactory);
    //	}
   // 	);

    return _instance.get();
}

std::unique_ptr<AttributeMeter> AttributeMeterFactory::create(const std::string& name)
{
	auto it = _factories.find(name);
	if(it != _factories.end())
	{
		SLOG_INFO( << "Found attribute")
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
}