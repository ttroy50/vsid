/**
 * Configuration for the VSID Pcap Classification program
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_PCAP_CLASSIFIER_CONFIG_H__
#define __VSID_PCAP_CLASSIFIER_CONFIG_H__

#include <string>
#include <memory>
#include <mutex>

namespace VsidPcapClassifier
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

private:
	static std::unique_ptr<Config> _instance;
	static std::once_flag _onceFlag;

	Config();
	Config(const Config& other) {}
	Config& operator=(const Config& rhs) {}
	

	std::string _config_file;
	std::string _protocol_database;
};


} // end namespace

#endif // END HEADER GUARD