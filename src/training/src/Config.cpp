/**
 * Config
 * 
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#include "Config.h"
#include "Logger.h"

using namespace VSID_TRAINING;
using namespace std;


Config* Config::_instance = NULL;

Config::Config() :
	_spid_database("spid_db.yaml")
{

}

Config::~Config()
{
	delete _instance;
	_instance = NULL;
}

Config* Config::instance()
{
	if(_instance == NULL)
	{
		_instance = new Config();
	}
	return _instance;
}

bool Config::init(const string& config_file)
{
	SLOG_INFO(<< "init config from config file : " << config_file);
	_config_file = config_file;
	return true;
}