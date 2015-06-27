/**
 * Configuration for the VSID Netfilter program
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_NETFILTER_CONFIG_H__
#define __VSID_NETFILTER_CONFIG_H__

#include <string>

namespace VsidNetfilter
{

/**
 * Config Singleton
 */
class Config
{
public:
	/**
	 * Get an instance of the singleton. 
	 * Should be called first from main thread
	 * @return
	 */
	static Config* instance();

	/**
	 * Initialise the config 
	 * @param config_file
	 */
	bool init(const std::string& config_file);

	void protocolDatabase(const std::string& s) { _protocol_database = s; }
	std::string protocolDatabase() { return _protocol_database; }

	uint32_t udpFlowTimeout() { return _udp_flow_timeout; }
	
	uint32_t numQueues() { return _num_queues; }
	uint32_t queueOffset() { return _queue_offset; }

	size_t nfQueueSize() { return _nf_queue_size; }
	size_t nfBufSize() { return _nf_buf_size; }
	
private:
	Config();
	~Config();

	static Config* _instance;

	std::string _config_file;
	std::string _protocol_database;

	uint32_t _udp_flow_timeout;

	uint32_t _num_queues;
	uint32_t _queue_offset;

	size_t _nf_queue_size;
	size_t _nf_buf_size;
};


} // end namespace

#endif // END HEADER GUARD