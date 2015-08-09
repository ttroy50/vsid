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
#include <memory>
#include <mutex>

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
	~Config() {};

	/**
	 * Initialise the config 
	 * @param config_file
	 */
	bool init(const std::string& config_file);

	uint32_t numQueues() { return _num_queues; }
	uint32_t queueOffset() { return _queue_offset; }

	size_t nfQueueSize() { return _nf_queue_size; }
	size_t nfBufSize() { return _nf_buf_size; }

private:
	static std::unique_ptr<Config> _instance;
	static std::once_flag _onceFlag;

	Config();
	Config(const Config& other) {}
	Config& operator=(const Config& rhs) {}

	std::string _config_file;

	uint32_t _num_queues;
	uint32_t _queue_offset;

	size_t _nf_queue_size;
	size_t _nf_buf_size;
};


} // end namespace

#endif // END HEADER GUARD