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

namespace VSID_TRAINING
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

	void spidDatabase(const std::string& s) { _spid_database = s; }
	std::string spidDatabase() { return _spid_database; }

	void spidDatabaseBackup(const std::string& s) { _spid_database_backup = s; }
	std::string spidDatabaseBackup() { return _spid_database_backup; }

private:
	Config();
	~Config();

	static Config* _instance;

	std::string _config_file;
	std::string _spid_database;
	std::string _spid_database_backup;
};


} // end namespace

#endif // END HEADER GUARD