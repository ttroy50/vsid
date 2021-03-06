/**
 * CommonConfig
 * 
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#include "CommonConfig.h"
#include "Logger.h"
#include "yaml-cpp/yaml.h"

using namespace VsidCommon;
using namespace std;


std::unique_ptr<CommonConfig> CommonConfig::_instance;
std::once_flag CommonConfig::_onceFlag;

CommonConfig* CommonConfig::instance()
{
    try
    {
        std::call_once(_onceFlag,
            [] {
        if(!_instance) 
                _instance.reset(new CommonConfig);
            }
        );
    }
    catch(std::runtime_error& e)
    {
        // not thread safe but this should be initialised alread in main thread
        // Not sure why I was sometimes seeing exception from call_once. Need time to investigate
        if(!_instance)
            _instance.reset(new CommonConfig);
    }

    return _instance.get();
}

CommonConfig::CommonConfig() :
	_learning_mode(false),
    _udp_flow_timeout(120),
    _divergence_threshold(2),
    _worker_threads_per_queue(0),
    _tcp_flow_close_wait_timeout(2),
    _tcp_flow_timeout(86400),
    _use_best_match(true),
    _use_port_hints(true),
    _worker_thread_queue_size(2048)
{

}