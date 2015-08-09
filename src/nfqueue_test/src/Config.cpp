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

using namespace VsidNetfilter;
using namespace VsidCommon;
using namespace std;


std::unique_ptr<Config> Config::_instance;
std::once_flag Config::_onceFlag;

Config* Config::instance()
{
    std::call_once(_onceFlag,
        [] {
    if(!_instance) 
            _instance.reset(new Config);
        }
    );

    return _instance.get();
}

Config::Config() :
    _num_queues(1),
    _queue_offset(0),
    _nf_queue_size(2048),
    _nf_buf_size(2048)
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



    if(config["NumQueues"])
    {
        _num_queues = config["NumQueues"].as<uint32_t>();
    }
    SLOG_INFO(<< "NumQueues : " << _num_queues)

    if(config["QueueOffset"])
    {
        _queue_offset = config["QueueOffset"].as<uint32_t>();
    }
    SLOG_INFO(<< "QueueOffset : " << _queue_offset)


    if(config["NfQueueSize"])
    {
        _nf_queue_size = config["NfQueueSize"].as<size_t>();
    }
    SLOG_INFO(<< "NfQueueSize : " << _nf_queue_size)



    if(config["NfBufSize"])
    {
        _nf_buf_size = config["NfBufSize"].as<size_t>();    
    }
    SLOG_INFO(<< "NfBufSize : " << _nf_buf_size)


	return true;
}