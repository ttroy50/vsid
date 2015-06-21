/**
 * Config
 * 
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#include "Config.h"
#include "Logger.h"
#include "yaml-cpp/yaml.h"

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

	if(_config_file.empty())
	{
		return false;
		SLOG_ERROR(<< "No config file specified");
	}

	// parse the atom/yaml...
    YAML::Node config;
    try 
    {
        config = YAML::LoadFile(config_file);
    } 
    catch (YAML::Exception& e)
    {
        LOG_ERROR(("Exception loading file [%v] into YAML [%v]", config_file, e.what()));
        return false;
    }

    if(config["SpidDatabase"])
    {
    	_spid_database = config["SpidDatabase"].as<string>();
    	SLOG_INFO(<< "SpidDatabase : " << _spid_database)
    }

    if(config["SpidDatabase"])
    {
    	_spid_database = config["SpidDatabase"].as<string>();
    	SLOG_INFO(<< "SpidDatabase : " << _spid_database)
    }

    if(config["SpidDatabaseBackup"])
    {
    	_spid_database_backup = config["SpidDatabaseBackup"].as<string>();
    	SLOG_INFO(<< "SpidDatabaseBackup : " << _spid_database_backup)
    }

	return true;
}