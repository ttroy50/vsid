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

	uint32_t tcpFlowTimeout() { return _tcp_flow_timeout; }
	void tcpFlowTimeout(uint32_t t) { _tcp_flow_timeout = t; }

	uint32_t tcpFlowCloseWaitTimeout() { return _tcp_flow_close_wait_timeout; }
	void tcpFlowCloseWaitTimeout(uint32_t t) { _tcp_flow_close_wait_timeout = t; }

	double divergenceThreshold() { return _divergenceThreshold; }
	void divergenceThreshold(double d) { _divergenceThreshold = d; }

	/**
	 * Number of worker threads per queue.
	 * The default is 0 which means it is all processed in the same thread it is read. 
	 * If using threads make sure to copy the buffer before passing a packet to a flowManager
	 * @return
	 */
	int workerThreadsPerQueue() { return _workerThreadsPerQueue; }
	void workerThreadsPerQueue(int n) { _workerThreadsPerQueue = n; }

private:
	static std::unique_ptr<CommonConfig> _instance;
	static std::once_flag _onceFlag;

	CommonConfig();
	CommonConfig(const CommonConfig& other) {}
	CommonConfig& operator=(const CommonConfig& rhs) {}

	bool _learning_mode;
	uint32_t _udp_flow_timeout;
	uint32_t _tcp_flow_timeout;
	uint32_t _tcp_flow_close_wait_timeout;
	double _divergenceThreshold;
	int _workerThreadsPerQueue;
};


} // end namespace

#endif // END HEADER GUARD