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

	/**
	 * Time that a UDP flow will be considered timed out if no packets are seen
	 * @return
	 */
	uint32_t udpFlowTimeout() { return _udp_flow_timeout; }
	void udpFlowTimeout(uint32_t t) { _udp_flow_timeout = t; }

	/**
	 * Time that a TCP flow will be considered timed out if no packets are seen
	 * @return
	 */
	uint32_t tcpFlowTimeout() { return _tcp_flow_timeout; }
	void tcpFlowTimeout(uint32_t t) { _tcp_flow_timeout = t; }

	/**
	 * Time that a TCP flow will be considered timed out if no packets are seen
	 * When it it in the finished state
	 * @return
	 */
	uint32_t tcpFlowCloseWaitTimeout() { return _tcp_flow_close_wait_timeout; }
	void tcpFlowCloseWaitTimeout(uint32_t t) { _tcp_flow_close_wait_timeout = t; }

	/**
	 * The Divergence Threshold for the K-L matching
	 * @return
	 */
	double divergenceThreshold() { return _divergence_threshold; }
	void divergenceThreshold(double d) { _divergence_threshold = d; }

	/**
	 * If set will use the Best Match Divergence from before the defining
	 * limit as a classification result after we get to the limit
	 * @return
	 */
	bool useBestMatch() { return _use_best_match; }
	void useBestMatch(bool b) { _use_best_match = b; }

	/**
	 * Number of worker threads per queue.
	 * The default is 0 which means it is all processed in the same thread it is read. 
	 * If using threads make sure to copy the buffer before passing a packet to a flowManager
	 * @return
	 */
	int workerThreadsPerQueue() { return _worker_threads_per_queue; }
	void workerThreadsPerQueue(int n) { _worker_threads_per_queue = n; }

	/**
	 * Size of each worker threads lockfree queue
	 * @return
	 */
	size_t workerThreadQueueSize() { return _worker_thread_queue_size; }
	void workerThreadQueueSize(size_t s) { _worker_thread_queue_size = s; }

	bool usePortHints() { return _use_port_hints; }
	void usePortHints(bool b) { _use_port_hints = b; }

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
	double _divergence_threshold;
	bool _use_best_match;
	int _worker_threads_per_queue;
	size_t _worker_thread_queue_size;
	bool _use_port_hints;
};


} // end namespace

#endif // END HEADER GUARD