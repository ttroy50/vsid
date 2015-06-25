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

using namespace VsidTraining;
using namespace std;


Config* Config::_instance = NULL;

Config::Config() :
	_protocol_database("protocol_model_db.yaml"),
	_udp_flow_timeout(120)
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

    if(config["ProtocolDatabaase"])
    {
    	_protocol_database = config["ProtocolDatabaase"].as<string>();
    	SLOG_INFO(<< "ProtocolDatabaase : " << _protocol_database)
    }

    if(config["ProtocolDatabaaseBackup"])
    {
    	_protocol_database_backup = config["ProtocolDatabaaseBackup"].as<string>();
    	SLOG_INFO(<< "ProtocolDatabaaseBackup : " << _protocol_database_backup)
    }

    if(config["UdpFlowTimeout"])
    {
    	_udp_flow_timeout = config["UdpFlowTimeout"].as<uint32_t>();
    	SLOG_INFO(<< "UdpFlowTimeout : " << _udp_flow_timeout)
    }

	return true;
}