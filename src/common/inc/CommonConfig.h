/**
 * Configuration for the VSID program
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_COMMON_CONFIG_H__
#define __VSID_COMMON_CONFIG_H__

#include <string>
#include <memory>
#include <mutex>

namespace VsidCommon
{

/**
 * Config Singleton
 */
class CommonConfig
{
public:
	/**
	 * Get an instance of the singleton. 
	 * Should be called first from main thread
	 * @return
	 */
	static CommonConfig* instance();
	~CommonConfig() {};

	void learningMode(bool s) { _learning_mode = s; }
	bool learningMode() { return _learning_mode; }

	uint32_t udpFlowTimeout() { return _udp_flow_timeout; }
	void udpFlowTimeout(uint32_t t) { _udp_flow_timeout = t; }

private:
	static std::unique_ptr<CommonConfig> _instance;
	static std::once_flag _onceFlag;

	CommonConfig();
	CommonConfig(const CommonConfig& other) {}
	CommonConfig& operator=(const CommonConfig& rhs) {}

	bool _learning_mode;
	uint32_t _udp_flow_timeout;
};


} // end namespace

#endif // END HEADER GUARD