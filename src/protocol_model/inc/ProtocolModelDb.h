/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_PROTOCOL_MODEL_DB_H__
#define __VSID_PROTOCOL_MODEL_DB_H__

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "ProtocolModel.h"
 

#include "yaml-cpp/yaml.h"
#include "boost/date_time/posix_time/posix_time.hpp"

namespace Vsid
{


#define PROTOCOL_MODEL_VERSION "0.2"

/**
 * The Protocol Model Database. 
 *
 * The class holds the models and fingerprints for all supported protocols, which are
 * read in from a YAML formatted file
 */
class ProtocolModelDb
{
public:
	/**
	 * Constructor. 
	 *
	 * @param filename 	The filename to read the db from and write the new one to
	 * @param backupfile The filename to backup the old DB to on write
	 */
	ProtocolModelDb(std::string filename, std::string backupfile);

	/**
	 * Read the protocol model from file into the database. 
	 *
	 * This can only happen once per program run and the current supported DB format
	 * is in YAML
	 *
	 * @return
	 */
	bool read();

	/**
	 * Writes the current DB to a YAML file. 
	 * 
	 * When writing will copy the old file to _filename.prev
	 *
	 * @return
	 */
	bool write();

	/**
	 * Find a model by name
	 * 
	 * @param name
	 * @return
	 */
	std::shared_ptr<ProtocolModel> find(const std::string& name);

	/**
	 * 	Get the first model in the list
	 *
	 *  @param port The port that you are looking for. 0 means default list
	 *  @return
	 */
	std::shared_ptr<ProtocolModel> front(uint16_t port = 0);

	/**
	 * Get the protocol model at a certain position
	 *
	 * @param pos The position in the list you get
	 * @param port The port that you are looking for. 0 means default list
	 * 
	 * @return
	 */
	std::shared_ptr<ProtocolModel> at(size_t pos, uint16_t port = 0);

	/**
	 * Get a count for the number of models
	 *
	 * @return
	 */
	size_t size() const { return _protocolModelOrder.size(); }

	/**
	 * Get the defining limit
	 * Protocols will not be hard matched before this limit
	 * @return
	 */
	uint32_t definingLimit() const { return _definingLimit; }

	/**
	 * Get the cutoffLimit
	 *
	 * The cutoff limit is the maximum number of packets to look at 
	 * in a flow. This is used for both classification and model creation
	 * 
	 * @return
	 */
	uint32_t cutoffLimit() const { return _cutoffLimit; }

	/**
	 * Filename of the database
	 * @return
	 */
	std::string filename() const { return _filename; }

	/**
	 * Time the DB was last modified / updated
	 * @return
	 */
	boost::posix_time::ptime lastModifiedTime() const;
	
	std::string lastModifiedTimeAsString() const;

	std::string databaseVersion() const { return _version; }
	
private:
	std::string _filename;
	std::string _backupfile;
	std::string _version;
	bool _initialised;

	uint32_t _definingLimit;
	uint32_t _cutoffLimit;
	std::vector<boost::posix_time::ptime> _modifiedTimes;
	
	std::vector<std::shared_ptr<ProtocolModel> > _protocolModelOrder;
	//typedef std::map<std::string, std::shared_ptr<ProtocolModel> > ProtocolModelMap;
	std::map<std::string, std::shared_ptr<ProtocolModel> > _protocolModels;
	//typedef std::map<uint16_t, std::vector<std::shared_ptr<ProtocolModel> > > PortHintProtocolModelMap;
	std::map<uint16_t, std::vector<std::shared_ptr<ProtocolModel> > > _portHintOrder;

	/**
	 * Read an attribute meter from the file
	 *
	 * @param node 
	 * @param count The count of this meter in the list
	 * 
	 * @return The AttributeMeter or a NULL pointer
	 */
	std::shared_ptr<AttributeMeter> _readAttributeMeter(const YAML::Node& node, const int count);
	
	/**
	 * Read an protcol model from the file
	 *
	 * @param node 
	 * @param count The count of this model in the list
	 * 
	 * @return The ProtocolModel or a NULL pointer
	 */
	std::shared_ptr<ProtocolModel> _readProtocolModel(const YAML::Node& node, const int count);

	bool _writeProtocolModel(YAML::Node& ym, std::shared_ptr<ProtocolModel> model);

	bool _writeAttributeMeter(YAML::Node& ym, std::shared_ptr<AttributeMeter> model);
};



} // end namespace

#endif // END HEADER GUARD