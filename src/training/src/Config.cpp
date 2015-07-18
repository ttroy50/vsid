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
#include "CommonConfig.h"

using namespace VsidTraining;
using namespace VsidCommon;
using namespace std;

std::unique_ptr<Config> Config::_instance;
std::once_flag Config::_onceFlag;

Config* Config::instance()
{
    try
    {
        std::call_once(_onceFlag,
            [] {
        if(!_instance) 
                _instance.reset(new Config);
            }
        );
    }
    catch(std::runtime_error& e)
    {
        // not thread safe but this should be initialised alread in main thread
        // Not sure why I was sometimes seeing exception from call_once. Need time to investigate
        if(!_instance)
            _instance.reset(new Config);
    }

    return _instance.get();
}

Config::Config() :
	_protocol_database("protocol_model_db.yaml")
{

}

bool Config::init(const string& config_file)
{
	SLOG_INFO(<< "init config from config file : " << config_file);
	_config_file = config_file;

    CommonConfig::instance()->learningMode(true);

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

    if(config["ProtocolDatabase"])
    {
    	_protocol_database = config["ProtocolDatabase"].as<string>();
    	SLOG_INFO(<< "ProtocolDatabase : " << _protocol_database)
    }

    if(config["ProtocolDatabaseBackup"])
    {
    	_protocol_database_backup = config["ProtocolDatabaseBackup"].as<string>();
    	SLOG_INFO(<< "ProtocolDatabaseBackup : " << _protocol_database_backup)
    }

    if(config["UdpFlowTimeout"])
    {
    	CommonConfig::instance()->udpFlowTimeout(config["UdpFlowTimeout"].as<uint32_t>());
    	SLOG_INFO(<< "UdpFlowTimeout : " << CommonConfig::instance()->udpFlowTimeout())
    }

	return true;
}