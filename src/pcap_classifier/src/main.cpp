/**
 * Main for pcap classifier program
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
#include "ProtocolModelDb.h"
#include "AttributeMeterRegistrar.h"
#include "FlowClassificationLogger.h"

using namespace std;
using namespace VsidPcapClassifier;
using namespace Vsid;
using namespace VsidCommon;

static const char* const gvProgramName = "VSID - PCAP Classifier Program";

INIT_LOGGING

void usage(const char* command)
{
	cout << endl << gvProgramName <<  endl << endl;
	cout << "Usage : " << command << " [-h] [-c <config_file>] [-d database_file] -p pcap file";
	cout << "Options :" << endl 
		<< " -c \t Config file " << endl
		<< " -l \t Logging Config " << endl
		<< " -d \t VSI SPID database file. Overrides the one from config file" << endl
		<< " -p \t PCAP file which to read" << endl
		<< " -h \t print this help and exit" << endl;

}


int main( int argc, char* argv[] )
{
	el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);

	string protocol_db;
	bool protocol_db_set = false;
	string config = "config.yaml";
	string pcap_file;
	string logging_conf = "../config/logging_pcap_classifier.conf";

	char c;
	while ( (c = getopt(argc, argv, ":hc:d:p:l") ) != -1) 
	{
	    switch (c) 
	    {
	    	case 'd':
	            protocol_db = optarg;
	            break;
	        case 'c':
	            config = optarg;
	            break;
	        case 'p':
	            pcap_file = optarg;
	            break;
	        case 'h':
	        	usage(argv[0]);
	        	exit(0);
	        	break;
	        case 'l':
	        	logging_conf = optarg;
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

	el::Configurations conf(logging_conf); 
	el::Loggers::reconfigureAllLoggers(conf);

	SLOG_INFO(	<< endl 
				<< "\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+" << endl
	   			<< "\t\t+ Running " << argv[0] << endl
				<< "\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+" << endl);


	init_attribute_meters();

    std::ifstream tmp(pcap_file);
    if ( !tmp.good() )
    {
        LOG_ERROR(("Unable to find pcap file : %v", pcap_file));
        cerr << "ERROR: Unable to find pcap file : " << pcap_file << endl;
        exit(1);
    }

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

	ProtocolModelDb protocolModelDb(Config::instance()->protocolDatabase(), "" );
	
	if( !protocolModelDb.read() )
	{
		SLOG_ERROR(<< "Unable to read DB at [" << Config::instance()->protocolDatabase() << "]")
		cerr << "Unable to read DB at [" << Config::instance()->protocolDatabase() << "]" << endl;
		exit(1);
	}

	SLOG_INFO(<< "spid database : " << Config::instance()->protocolDatabase());

	FlowManager flowManager(&protocolModelDb);
	flowManager.init();
	
	PcapReader reader(&flowManager);

	// Start the trace file for classification
	SLOG_TRACE( << "---"
				<< std::endl
				<< "ProtocolDatabase: {" 
				<< "FileName: " << protocolModelDb.filename() 
				<< ", LastModifiedTime: " << protocolModelDb.lastModifiedTimeAsString()
				<< ", CutoffLimit: " << protocolModelDb.cutoffLimit()
				<< ", DefiningLimit: " << protocolModelDb.definingLimit()
				<< "}"
				<< std::endl
				<< "Results: ");

	FlowClassificationLogger fcLogger(&flowManager);

	if ( !reader.read(pcap_file) )
	{
		SLOG_ERROR(<< "Unable to read pcap [" << pcap_file << "]");
	}
	else
	{
        flowManager.finished();
	}

	SLOG_INFO(<< "Finished program")
}