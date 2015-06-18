/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_METER_FACTORY_H__
#define __VSID_ATTRIBUTE_METER_FACTORY_H__

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <map>
#include "AttributeMeter.h"
#include <iostream>
namespace Vsid
{

typedef std::unique_ptr<AttributeMeter> (*CreateFn)();

/**
 * Factory for creating attribute meters
 */
class AttributeMeterFactory
{
public:
	static AttributeMeterFactory* instance();
	~AttributeMeterFactory() {}

	std::unique_ptr<AttributeMeter> create(const std::string& name);

	void registerFactory(const std::string& name, CreateFn fn);

private:
	static std::unique_ptr<AttributeMeterFactory> _instance;
	static std::once_flag _onceFlag;

	AttributeMeterFactory() {}
	AttributeMeterFactory(const AttributeMeterFactory& other) {}
	AttributeMeterFactory& operator=(const AttributeMeterFactory& rhs) {}
	

	
	typedef std::map<std::string, std::unique_ptr<AttributeMeter> (*)()> FactoryMap;
	FactoryMap _factories;
	
};


/**
 * Should be a static public member of the AttributeMeter sub classes. 
 *
 * A call to the register function should be added to the init_attribute_meter function
 * in AttributeMeterRegistrar
 *
 * I'm not a fan of how I'm doing this but it gets me around the static initialization
 * problems I was having trying to do automatic registration.
 */
class Registrar 
{ 
public: 
    Registrar(std::string className, Vsid::CreateFn fn) :
    	_className(className),
    	_fn(fn)
	{ } 

	/**
	 * Register the attribute meter with the factory
	 */
	void doRegistration()
	{
		Vsid::AttributeMeterFactory::instance()->registerFactory(_className, _fn); 
	}

private:
	std::string _className;
	Vsid::CreateFn _fn;
}; 

} // end namespace

#endif // END HEADER GUARD