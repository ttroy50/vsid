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
#include "ProtocolModelUpdater.h"

using namespace std;
using namespace VsidTraining;
using namespace Vsid;
using namespace VsidCommon;

static const char* const gvProgramName = "VSID - SPID Training Program";

INIT_LOGGING

void usage(const char* command)
{
	cout << endl << gvProgramName <<  endl << endl;
	cout << "Usage : " << command << " [-h] [-c <config_file>] [-d <database_file>] [-l <logging_config>] -t <training_file>" << endl;
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
	el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);

	string protocol_db;
	bool protocol_db_set = false;
	string config = "config.yaml";
	string training;
    string log_config = "../config/logging.conf";
	char c;
	while ( (c = getopt(argc, argv, ":hc:d:t:l:") ) != -1) 
	{
	    switch (c) 
	    {
	    	case 'd':
	            protocol_db = optarg;
	            break;
	        case 'c':
	            config = optarg;
	            break;
	        case 't':
	            training = optarg;
	            break;
            case 'l':
                log_config = optarg;
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

    el::Configurations conf(log_config); 
    el::Loggers::reconfigureAllLoggers(conf);

    SLOG_INFO(  << endl 
                << "\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+" << endl
                << "\t\t+ Running " << argv[0] << endl
                << "\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+" << endl);


	init_attribute_meters();

    

    SLOG_INFO(<< "config file : " << config);

	if ( !Config::instance()->init(config) )
	{
		SLOG_ERROR( << "Unable to initialise configuration");
		cerr << "ERROR: Unable to initialise configuration" << endl;
		exit(1);
	}

	if( !protocol_db.empty() )
	{
		SLOG_INFO( << "overriding protocol_db from config : " << protocol_db); 
		Config::instance()->protocolDatabase(protocol_db);
	}

	if(Config::instance()->protocolDatabase().empty())
	{
		SLOG_ERROR(<< "No spid database configured")
		cerr << "No spid database configured" << endl;
		exit(1);
	}

	ProtocolModelDb protocolModelDb(Config::instance()->protocolDatabase(), 
										Config::instance()->protocolDatabaseBackup() );
	
	if( !protocolModelDb.read() )
	{
		SLOG_ERROR(<< "Unable to read DB at [" << Config::instance()->protocolDatabase() << "]")
		cerr << "Unable to read DB at [" << Config::instance()->protocolDatabase() << "]" << endl;
		exit(1);
	}

	SLOG_INFO(<< "spid database : " << Config::instance()->protocolDatabase());

	
    TrainingInput training_input;
    if ( !training_input.read(training, &protocolModelDb) )
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

	FlowManager flowManager(&protocolModelDb);
    flowManager.init();
    
    ProtocolModelUpdater pmUpdated(&flowManager, &protocolModelDb);

	PcapReader reader(&flowManager);

	bool updated = false;
	for(int i = 0; i < training_input.trainingFiles().size(); i++)
	{
		if(training_input.trainingFiles()[i].exists)
		{

            pmUpdated.setCurrentFile(&training_input.trainingFiles()[i]);

			if ( !reader.read(training_input.trainingFiles()[i].filename) )
			{
				SLOG_ERROR(<< "Unable to read pcap [" << training_input.trainingFiles()[i].filename << "]");
			}
			else
			{
                cout << i+1 << " of " << training_input.trainingFiles().size() << " files processed" << endl;
                // Tell the flow manager that it can cleanup flows before the next round
                // This will update the protocol model from a callback to ProtocolModelUpdater
                flowManager.finished();
				updated = true;
			}
		}
	}

	if(pmUpdated.updated())
	{
		protocolModelDb.write();
	}
	else
	{
		SLOG_INFO(<< "protocol model not updated");
	}

	SLOG_INFO(<< "Finished program")
}