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

using namespace VsidNetfilter;
using namespace std;


Config* Config::_instance = NULL;

Config::Config() :
	_protocol_database("protocol_model_db.yaml"),
	_udp_flow_timeout(120),
    _num_queues(1),
    _queue_offset(0),
    _nf_queue_size(2048),
    _nf_buf_size(2048)
{

}

Config::~Config()
{
    if(_instance)
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


    if(config["UdpFlowTimeout"])
    {
    	_udp_flow_timeout = config["UdpFlowTimeout"].as<uint32_t>();
    	SLOG_INFO(<< "UdpFlowTimeout : " << _udp_flow_timeout)
    }

    if(config["NumQueues"])
    {
        _num_queues = config["NumQueues"].as<uint32_t>();
        SLOG_INFO(<< "NumQueues : " << _num_queues)
    }

    if(config["QueueOffset"])
    {
        _queue_offset = config["QueueOffset"].as<uint32_t>();
        SLOG_INFO(<< "QueueOffset : " << _queue_offset)
    }

    if(config["NfQueueSize"])
    {
        _udp_flow_timeout = config["NfQueueSize"].as<size_t>();
        SLOG_INFO(<< "NfQueueSize : " << _nf_queue_size)
    }

    if(config["NfBufSize"])
    {
        _nf_buf_size = config["NfBufSize"].as<size_t>();
        SLOG_INFO(<< "NfBufSize : " << _nf_buf_size)
    }
    
	return true;
}