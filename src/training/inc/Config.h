/**
 * Configuration for the VSID Training program
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_TRAINING_CONFIG_H__
#define __VSID_TRAINING_CONFIG_H__

#include <string>
#include <memory>
#include <mutex>

namespace VsidTraining
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

	void protocolDatabase(const std::string& s) { _protocol_database = s; }
	std::string protocolDatabase() { return _protocol_database; }

	const std::string& protocolDatabaseBackup() { return _protocol_database_backup; }

	uint32_t udpFlowTimeout() { return _udp_flow_timeout; }

private:
	static std::unique_ptr<Config> _instance;
	static std::once_flag _onceFlag;

	Config();
	Config(const Config& other) {}
	Config& operator=(const Config& rhs) {}
	

	std::string _config_file;
	std::string _protocol_database;
	std::string _protocol_database_backup;
	uint32_t _udp_flow_timeout;

};


} // end namespace

#endif // END HEADER GUARD