/**
 * Main for netfilter program
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
#include "PacketHandler.h"

using namespace std;
using namespace VsidNetfilter;
//using namespace Vsid;

static const char* const gvProgramName = "VSID - Netfilter Program";

INIT_LOGGING

void usage(const char* command)
{
	cout << endl << gvProgramName <<  endl << endl;
	cout << "Usage : " << command << " [-h] [-c <config_file>] [-d database_file]";
	cout << "Options :" << endl 
		<< " -c \t Config file " << endl
		<< " -l \t Logging Config " << endl
		<< " -d \t VSI SPID database file. Overrides the one from config file" << endl
		<< " -h \t print this help and exit" << endl;

}


int main( int argc, char* argv[] )
{
	// TODO read from argv	
	el::Configurations conf("../config/logging_netfilter.conf"); 
	el::Loggers::reconfigureAllLoggers(conf);

	SLOG_INFO(	<< endl 
				<< "\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+" << endl
	   			<< "\t\t+ Running " << argv[0] << endl
				<< "\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+" << endl);

	string protocol_db;
	bool protocol_db_set = false;
	string config = "config.yaml";
	string training;

	char c;
	while ( (c = getopt(argc, argv, ":hc:d:") ) != -1) 
	{
	    switch (c) 
	    {
	    	case 'd':
	        	SLOG_INFO(<< "-d " << optarg);
	            protocol_db = optarg;
	            break;
	        case 'c':
	        	SLOG_INFO(<< "-c " << optarg);
	            config = optarg;
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

	//init_attribute_meters();

    /*SLOG_INFO(<< "config file : " << config);

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
	*/

	PacketHandler reader;
	reader.run();

	SLOG_INFO(<< "Finished program")
}