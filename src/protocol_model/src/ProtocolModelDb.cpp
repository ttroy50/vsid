#include <iostream>

#include "ProtocolModelDb.h"
#include "ProtocolModel.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"
#include "yaml-cpp/yaml.h"
#include "CommonConfig.h"

using namespace std;
using namespace Vsid;

ProtocolModelDb::ProtocolModelDb(string filename, string backupfile) :
	_filename(filename),
	_initialised(false),
	_cutoffLimit(100),
	_definingLimit(10)
{
	if(backupfile.empty())
	{
		_backupfile = _filename + ".prev";
	}
	else
	{
		_backupfile = backupfile;
	}
}

bool ProtocolModelDb::read()
{
	if( _initialised )
	{
		SLOG_ERROR(<< "Unable to read DB because it is already initialised");
		return false;
	}

	// parse the atom/yaml...
	YAML::Node config;
	try 
	{
		std::ifstream tmp(_filename);
		if ( !tmp.good() )
		{
			SLOG_ERROR(<< "Unable to find vsid db file : " << _filename);
			tmp.close();
			return false;
		}
		tmp.close();
		config = YAML::LoadFile(_filename);
	} 
	catch (YAML::Exception& e)
	{
		LOG_ERROR(( "Exception loading file [%v] into YAML [%v]", _filename, e.what() ));
		return false;
	}

	try
	{
		_definingLimit = config["DefiningLimit"].as<uint32_t>();
		SLOG_INFO(<< "DefiningLimit : " << _definingLimit);

		_cutoffLimit = config["CutoffLimit"].as<uint32_t>();
		SLOG_INFO(<< "CutoffLimit : " << _cutoffLimit);
		if(_cutoffLimit > 0 && _cutoffLimit <= _definingLimit)
		{
			_cutoffLimit = _definingLimit + 1;
			SLOG_INFO(<< "CutoffLimit less than DefiningLimit. Setting to : " << _cutoffLimit);
		}

		if( config["Version"] )
		{
			_version = config["Version"].as<string>();
		}
		else
		{
			// v0.1 didn't include a version
			_version = "0.1";
		}
		
		SLOG_INFO(<< "Version : " << _version);
		if(_version != PROTOCOL_MODEL_VERSION )
		{
			// For now just warn. In future may want to updated
			SLOG_WARN(<< "Protocol Database versions don't match: Database [" 
							<< _version << "] : Program [" << PROTOCOL_MODEL_VERSION << "]");
		}

		if( config["ModifiedTime"] )
		{
			for (YAML::const_iterator it=config["ModifiedTime"].begin(); 
						it!=config["ModifiedTime"].end(); ++it)
			{
				try 
				{
					boost::posix_time::ptime tmp = boost::posix_time::from_iso_string(it->as<std::string>());
					_modifiedTimes.push_back(tmp);
				}
				catch(const std::exception& ex)
				{
					SLOG_ERROR(<< "Caught exception processing date " << ex.what());
				}
				catch(...)
				{
					SLOG_ERROR( << "Caught unknown exception processing date" );
				}
			}
		}
		else
		{
			SLOG_INFO(<< "No modified time in DB");
			boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
			_modifiedTimes.push_back(now);

		}

		int count = 0;
		int valid = 0;
		for (YAML::const_iterator it=config["ProtocolModels"].begin(); 
						it!=config["ProtocolModels"].end(); ++it)
		{
			std::shared_ptr<ProtocolModel> model = _readProtocolModel((*it), count);

			count++;

			if ( model )
			{
				valid++;
				
				if( _protocolModels.find(model->name()) != _protocolModels.end() )
				{
					SLOG_ERROR(<< "Model [" << model->name() << "] already exists in DB")
					continue;
				}
				_protocolModels[model->name()] = model;
				_protocolModelOrder.push_back(model);

				for(size_t i = 0; i < model->portHints().size(); ++i)
				{
					_portHintOrder[ model->portHints().at(i) ].push_back( model );
				}
				SLOG_INFO( << "Model [" << model->name() << "] added to DB");
			}
		}
	}
	catch (YAML::Exception& e)
	{
		LOG_ERROR(( "Exception readomg file [%v] into YAML [%v]", _filename, e.what() ));
		return false;
	}

	if(_protocolModelOrder.size() == 0)
	{
		SLOG_ERROR(<< "No protocol models in the DB")
	}

	// finish filling out the port order tables
	for(auto pmIt = _protocolModelOrder.begin(); pmIt != _protocolModelOrder.end(); ++pmIt )
	{

		for( auto orderIt = _portHintOrder.begin(); orderIt != _portHintOrder.end(); ++orderIt)
		{
			bool found = false;
			for(int i =0; i < orderIt->second.size(); i++)
			{
				if( (*pmIt)->name() == orderIt->second[i]->name() )
				{
					found = true;
					break;
				}
			}
			if ( !found )
			{
				orderIt->second.push_back((*pmIt));
			}

		}
	}
	
	/*for( auto orderIt = _portHintOrder.begin(); orderIt != _portHintOrder.end(); ++orderIt)
	{
		std::cout << "Order for : " << orderIt->first << endl;

		for(int i =0; i < orderIt->second.size(); i++)
		{
			cout << "\t" << orderIt->second[i]->name() << endl;
		}
	}*/
			
	SLOG_INFO( << "DB Initialised");
	_initialised = true;
	return true;
}

std::shared_ptr<ProtocolModel> ProtocolModelDb::_readProtocolModel(const YAML::Node& node, const int count)
{
	std::shared_ptr<ProtocolModel> model(new ProtocolModel());

	if( node["ProtocolName"] )
	{
		model->_name = node["ProtocolName"].as<string>();
		SLOG_INFO(<< "ProtocolName : " << model->_name);
	}
	else
	{
		SLOG_ERROR(<< "ProtocolModel at [" << count << "] doesn't have a ProtocolName");
		return nullptr;
	}

	if( node["FlowCount"] )
	{
		model->_flowCount = node["FlowCount"].as<uint32_t>();
		SLOG_INFO(<< "FlowCount : " << model->_flowCount);
	}
	else
	{
		SLOG_WARN(<< "ProtocolModel at [" << count << "] doesn't have a FlowCount");
		model->_flowCount = 0;
		SLOG_INFO(<< "FlowCount : " << model->_flowCount);
	}

	if( node["Enabled"] )
	{
		model->_enabled = node["Enabled"].as<bool>();
		SLOG_INFO(<< "Enabled : " << model->_enabled);
	}
	else
	{
		SLOG_WARN(<< "ProtocolModel at [" << count << "] doesn't have an Enabled flag");
		model->_enabled = false;
		SLOG_INFO(<< "Enabled : " << model->_enabled);
	}

	/*if( !model->_enabled )
	{
		SLOG_INFO(<< "ProtocolModel at [" << count << "] not enabled");
		return nullptr;
	}*/

	if( node["PortHints"] )
	{
		for (YAML::const_iterator phit=node["PortHints"].begin(); 
				phit!=node["PortHints"].end(); ++phit)
		{
			uint16_t hint = (*phit).as<uint16_t>();

			model->_portHints.push_back(hint);
			SLOG_INFO(<< "Added PortHint : " << hint);
		}
	}
	else
	{
		SLOG_WARN(<< "ProtocolModel at [" << count << "] doesn't have any PortHints");
	}

	if( node["AttributeMeters"] )
	{
		int amCount = 0;
		for (YAML::const_iterator atit=node["AttributeMeters"].begin(); 
				atit != node["AttributeMeters"].end(); ++atit)
		{
			std::shared_ptr<AttributeMeter> am = _readAttributeMeter((*atit), amCount);

			if( am )
			{
				SLOG_INFO(<< "Adding AttributeMeter [" << am->name() << "]");
				model->_attributeMeters.push_back(am);
			}
			else
			{
				SLOG_ERROR( << "Attribute Meter at [" << amCount << "] is not valid. Cannot add ProtocolModel to DB : " <<model->_name);
				return nullptr; 
			}
			amCount++;
		}
	}
	else
	{
		SLOG_WARN(<< "ProtocolModel at [" << count << "] doesn't have any AttributeMeters");
		return nullptr;
	}

	return model;
}

std::shared_ptr<AttributeMeter> ProtocolModelDb::_readAttributeMeter(const YAML::Node& node, const int count)
{
	
	string name;
	if( node["AttributeName"] )
	{
		name = node["AttributeName"].as<string>();
		SLOG_INFO(<< "AttributeName : " << name);
	}
	else
	{
		SLOG_ERROR(<< "AttributeMeter at [" << count << "] doesn't have a AttributeName");
		return nullptr;
	}

	SLOG_INFO(<< "Trying to create attr");
	//std::shared_ptr<AttributeMeter> attr(
	//							AttributeMeterFactory::instance()->create(name) 
	//						);
	auto attr = AttributeMeterFactory::instance()->create(name);
	//std::unique_ptr<AttributeMeter> uqp(new ByteFrequency());

	//auto attr = std::shared_ptr<AttributeMeter> { uqp };
	SLOG_INFO(<< "create attr");
	
	if( !attr )
	{
		SLOG_INFO(<< "Unable to create attribute meter from factory [" << name << "]" 
						<< " at [" << count << "]");
		return nullptr;
	}

	attr->_fromDb = true;

	if( node["Enabled"] )
	{
		attr->_enabled = node["Enabled"].as<bool>();
		SLOG_INFO(<< "Enabled : " << attr->_enabled);
	}
	else
	{
		SLOG_WARN(<< "AttributeMeter " << name << " at [" << count << "] doesn't have an Enabled flag");
		attr->_enabled = false;
		SLOG_INFO(<< "FlowCount : " << attr->_enabled);
	}

	// Used by at lease one protocol so globally enable the attribute meter 
	if( attr->_enabled )
	{
		AttributeMeterFactory::instance()->enableAttribute(name);
	}

	if( node["FlowCount"] )
	{
		attr->_flowCount = node["FlowCount"].as<uint32_t>();
		SLOG_INFO(<< "FlowCount : " << attr->_flowCount);
	}
	else
	{
		SLOG_WARN(<< "AttributeMeter at [" << count << "] doesn't have a FlowCount. Will be disabled");
		attr->_flowCount = 0;
		attr->_enabled = false;
		SLOG_INFO(<< "FlowCount : " << attr->_flowCount);
	}


	/*if( attr->_flowCount > 0)
	{
		attr->_klFixMultiplicator = (double) attr->flowCount() / (attr->size() + attr->flowCount());
		attr->_klFixIncrement = (1.0 / attr->flowCount()) / (attr->size() + attr->flowCount());
	}*/

	if( node["FingerPrint"] )
	{
		if(node["FingerPrint"].size() != attr->_fingerprint_size)
		{
			SLOG_ERROR(<< "Error in " << name 
						<< "Fingerprint size in DB [" << node["FingerPrint"].size() 
						<< "] different to size in the code [" 
						<< attr->_fingerprint_size << "]")
			return nullptr;
		}

		std::vector<double> fp;
		for (YAML::const_iterator phit=node["FingerPrint"].begin(); 
				phit!=node["FingerPrint"].end(); ++phit)
		{
			double print = (*phit).as<double>();

			fp.push_back(print);
			//SLOG_INFO(<< "Added FingerPrint : " << print);
		}
		attr->_fingerprint = fp;
	}
	else
	{
		SLOG_ERROR(<< "AttributeMeter " << name << " at [" << count << "] doesn't have any FingerPrint");
		return nullptr;
	}

	std::shared_ptr<AttributeMeter> tmp { std::move(attr) };
	return tmp;
}


bool ProtocolModelDb::write()
{
	SLOG_INFO(<< "Writing protocol model db to file");
	YAML::Node root;
	root["DefiningLimit"] = _definingLimit;
	root["CutoffLimit"] = _cutoffLimit;
	root["Version"] = PROTOCOL_MODEL_VERSION;

	boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

	_modifiedTimes.insert(_modifiedTimes.begin(), now);

	for(int i = 0; i < _modifiedTimes.size(); i++)
	{
		root["ModifiedTime"].push_back(boost::posix_time::to_iso_string(_modifiedTimes[i]));
	}

	for(auto it = _protocolModelOrder.begin(); 
				it != _protocolModelOrder.end(); ++it)
	{
		YAML::Node ym;
		_writeProtocolModel(ym, (*it));
		root["ProtocolModels"].push_back(ym);
	}

	YAML::Emitter out;
    // more compact form 
    out.SetMapFormat(YAML::Flow);
    out << root;

  	SLOG_INFO(<< "New DB : " << endl << out.c_str() << endl);

  	SLOG_INFO(<< "Backup file will be : " << _backupfile);

  	if( std::rename(_filename.c_str(), _backupfile.c_str()) == 0)
  	{
  		SLOG_INFO(<< "Have moved file");
  		ofstream file;
		file.open (_filename);
		if(file.is_open())
		{
			file << out.c_str();
			file.close();
			SLOG_INFO(<< "Have written new DB to " << _filename);
		}
		else
		{
			SLOG_ERROR(<< "Unable to write new DB. See " << _backupfile << "for DB");
			std::cerr << "Unable to write new DB. See " << _backupfile << "for DB" << std::endl;
			return false;
		}
  	}
  	else
  	{
  		SLOG_ERROR(<< "Unable to back old DB won't overwrite");

  		string errFile = _filename;
  		errFile += ".new";

  		ofstream file;
		file.open (errFile);
		if(file.is_open())
		{
			file << out.c_str();
			file.close();
			SLOG_INFO(<< "Have written new DB to " << errFile);
			return false;
		}
		else
		{
			return false;
			SLOG_ERROR(<< "Unable write new DB. See " << errFile << "for DB");
		}
  	}

  	return true;
}

bool ProtocolModelDb::_writeProtocolModel(YAML::Node& ym, std::shared_ptr<ProtocolModel> model)
{
	ym["ProtocolName"] = model->_name;
	ym["FlowCount"] = model->_flowCount;
	ym["Enabled"] = model->_enabled;

	YAML::Node ph;
	for(int i = 0; i < model->_portHints.size(); i++)
	{
		ph.push_back(model->_portHints[i]);
	}
	ym["PortHints"] = ph;

	for(auto it = model->_attributeMeters.begin();
			it != model->_attributeMeters.end(); ++it)
	{
		YAML::Node yam;
		_writeAttributeMeter(yam, (*it));
		ym["AttributeMeters"].push_back(yam);
	}
	return true;
}

bool ProtocolModelDb::_writeAttributeMeter(YAML::Node& ym, std::shared_ptr<AttributeMeter> model)
{
	ym["AttributeName"] = model->name();
	ym["FlowCount"] = model->_flowCount;
	ym["Enabled"] = model->_enabled;

	YAML::Node ph;
	for(int i = 0; i < model->_fingerprint.size(); i++)
	{
		ph.push_back(model->_fingerprint[i]);
	}
	ym["FingerPrint"] = ph;

	return true;
}

std::shared_ptr<ProtocolModel> ProtocolModelDb::find(const std::string& name)
{
	auto it = _protocolModels.find(name);
	if(it != _protocolModels.end())
		return it->second;
	else
		return nullptr;
}


std::shared_ptr<ProtocolModel> ProtocolModelDb::front(uint16_t port /* = 0 */)
{
	return at(0, 0);
}


std::shared_ptr<ProtocolModel> ProtocolModelDb::at(size_t pos, uint16_t port /* = 0 */)
{
	if(port == 0)
	{
		if(pos < _protocolModelOrder.size() )
		{
			return _protocolModelOrder[pos];
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		auto it = _portHintOrder.find(port);
		if(it != _portHintOrder.end())
		{
			if( pos < it->second.size() )
			{
				return it->second[pos];
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			if( pos < _protocolModelOrder.size() )
			{
				return _protocolModelOrder[pos];
			}
			else
			{
				return nullptr;
			}
		}
	}
}


boost::posix_time::ptime ProtocolModelDb::lastModifiedTime() const
{
	if(_modifiedTimes.size() > 0)
	{
		return _modifiedTimes[0];
	}
	else
	{
		return boost::posix_time::second_clock::universal_time();
	}
}
	
std::string ProtocolModelDb::lastModifiedTimeAsString() const
{
	return boost::posix_time::to_iso_string(lastModifiedTime());
}