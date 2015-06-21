/**
 * Main for training program
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */


#include <stdio.h>
#include <getopt.h>

#include <iostream>
#include <vector>

#include "Logger.h"
#include "Config.h"
#include "PcapReader.h"
#include "TrainingInput.h"
#include "ProtocolModelDb.h"
#include "AttributeMeterRegistrar.h"
 
using namespace std;
using namespace VSID_TRAINING;
using namespace Vsid;

static const char* const gvProgramName = "VSID - SPID Training Program";

INIT_LOGGING

void usage(const char* command)
{
	cout << endl << gvProgramName <<  endl << endl;
	cout << "Usage : " << command << " [-h] [-c <config_file>] [-d database_file] -t training_file";
	cout << "Options :" << endl 
		<< " -c \t Config file " << endl
		<< " -l \t Logging Config " << endl
		<< " -d \t VSI SPID database file. Overrides the one from config file" << endl
		<< " -t \t Training file which lists the pcap files to read" << endl
		<< " -h \t print this help and exit" << endl;

}


int main( int argc, char* argv[] )
{
	// TODO read from argv	
	el::Configurations conf("../config/logging.conf"); 
	el::Loggers::reconfigureAllLoggers(conf);

	SLOG_INFO(	<< endl 
				<< "\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+" << endl
	   			<< "\t\t+ Running " << argv[0] << endl
				<< "\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+" << endl);

	string spid_db;
	bool spid_db_set = false;
	string config = "config.yaml";
	string training;

	char c;
	while ( (c = getopt(argc, argv, ":hc:d:t:") ) != -1) 
	{
	    switch (c) 
	    {
	    	case 'd':
	        	SLOG_INFO(<< "-d " << optarg);
	            spid_db = optarg;
	            break;
	        case 'c':
	        	SLOG_INFO(<< "-c " << optarg);
	            config = optarg;
	            break;
	        case 't':
	        	SLOG_INFO(<< "-t " << optarg);
	            training = optarg;
	            break;
	        case 'h':
	        	usage(argv[0]);
	        	exit(0);
	        	break;
	        case ':':
	        	cerr << "ERROR: Option " << (char)optopt << " requires an argument." << endl;
	            usage(argv[0]);
	            exit(1);
	            break;
	        case '?':
	        default:
	        	cerr << "ERROR: Unrecognised Option : " << (char)optopt << endl;
	            usage(argv[0]);
	            exit(1);
	            break;
	    }
	}

	init_attribute_meters();

    TrainingInput training_input;
    if ( !training_input.read(training) )
    {
    	SLOG_ERROR( << "ERROR: Unable to read training file" );
    	exit(1);
    }

    if(training_input.trainingFiles().size() == 0)
    {
    	cerr << "ERROR: Need a pcap file to analyse" << endl;
    	usage(argv[0]);
    	exit(1);
    }
    else
    {
    	for(int i = 0; i < training_input.trainingFiles().size(); i++)
    	{
    		std::ifstream tmp(training_input.trainingFiles()[i].filename);
    		if ( !tmp.good() )
    		{

    			LOG_ERROR(("Unable to find pcap file : %v", training_input.trainingFiles()[i].filename));
    			cerr << "ERROR: Unable to find pcap file : " << training_input.trainingFiles()[i].filename << endl;
    			continue;
    		}
    		else
    		{
    			training_input.trainingFiles()[i].exists = true;
    		}
    	}
    }

    SLOG_INFO(<< "config file : " << config);

	if ( !Config::instance()->init(config) )
	{
		SLOG_ERROR( << "Unable to initialise configuration");
		cerr << "ERROR: Unable to initialise configuration" << endl;
		exit(1);
	}

	if( !spid_db.empty() )
	{
		SLOG_INFO( << "overriding spid_db from config : " << spid_db); 
		Config::instance()->spidDatabase(spid_db);
	}

	if(Config::instance()->spidDatabase().empty())
	{
		SLOG_ERROR(<< "No spid database configured")
		cerr << "No spid database configured" << endl;
		exit(1);
	}

	ProtocolModelDb protocolModelDb(Config::instance()->spidDatabase(), 
										Config::instance()->spidDatabaseBackup() );
	
	if( !protocolModelDb.read() )
	{
		SLOG_ERROR(<< "Unable to read DB at [" << Config::instance()->spidDatabase() << "]")
		cerr << "Unable to read DB at [" << Config::instance()->spidDatabase() << "]" << endl;
		exit(1);
	}

	SLOG_INFO(<< "spid database : " << Config::instance()->spidDatabase());

	PcapReader reader;

	bool updated = false;
	for(int i = 0; i < training_input.trainingFiles().size(); i++)
	{
		if(training_input.trainingFiles()[i].exists)
		{
			if ( !reader.read(training_input.trainingFiles()[i].filename) )
			{
				SLOG_ERROR(<< "Unable to read pcap [" << training_input.trainingFiles()[i].filename << "]");
			}
			else
			{
				updated = true;
			}
		}
	}

	/*if(updated)
	{
		protocolModelDb.write();
	}*/

	SLOG_INFO(<< "Finished program")
}