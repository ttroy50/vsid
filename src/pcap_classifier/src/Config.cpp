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

using namespace VsidPcapClassifier;
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

    CommonConfig::instance()->learningMode(false);

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

    if(config["UdpFlowTimeout"])
    {
    	CommonConfig::instance()->udpFlowTimeout(config["UdpFlowTimeout"].as<uint32_t>());
    	SLOG_INFO(<< "UdpFlowTimeout : " << CommonConfig::instance()->udpFlowTimeout())
    }

    if(config["KLDivergenceThreshold"])
    {
        CommonConfig::instance()->divergenceThreshold(config["KLDivergenceThreshold"].as<double>());
    }
    SLOG_INFO(<< "KLDivergenceThreshold : " << CommonConfig::instance()->divergenceThreshold())


    if(config["UseBestMatchDivergence"])
    {
        CommonConfig::instance()->useBestMatch(config["UseBestMatchDivergence"].as<bool>());
    }
     SLOG_INFO(<< "UseBestMatchDivergence : " << CommonConfig::instance()->useBestMatch())

    if(config["NumWorkerThreadsPerQueue"])
    {
        CommonConfig::instance()->workerThreadsPerQueue(config["NumWorkerThreadsPerQueue"].as<int>());
        SLOG_INFO(<< "NumWorkerThreadsPerQueue : " << CommonConfig::instance()->workerThreadsPerQueue())
    }
    else
    {
        CommonConfig::instance()->workerThreadsPerQueue(0);
        SLOG_INFO(<< "NumWorkerThreadsPerQueue : " << CommonConfig::instance()->workerThreadsPerQueue())
    }

	return true;
}