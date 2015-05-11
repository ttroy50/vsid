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
 
using namespace std;
using namespace VSID_TRAINING;

static const char* const gvProgramName = "VSID - SPID Training Program";

INIT_LOGGING

void usage(const char* command)
{
	cout << endl << gvProgramName <<  endl << endl;
	cout << "Usage : " << command << " [-h] [-c <config_file>] <pcap file> [<pcap file> ...]";
	cout << "Options :" << endl 
		<< " -c \t Config file " << endl
		<< " -d \t VSI SPID database file. Overrides the one from config file"
		<< " -h \t print this help and exit" << endl;

}


int main( int argc, char* argv[] )
{

	string spid_db;
	bool spid_db_set = false;
	string config = "config.yaml";
	std::vector<string> pcap_files;

	char c;
	while ( (c = getopt(argc, argv, ":hc:d:") ) != -1) 
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

	if (optind < argc) {
        SLOG_INFO(<< (argc - optind) << " pcap file(s) : ");
        while (optind < argc)
        {
        	pcap_files.push_back(argv[optind++]);
        	SLOG_INFO(<< "\t" << pcap_files.back());
        }
    }

    if(pcap_files.size() == 0)
    {
    	cerr << "ERROR: Need a pcap file to analyse" << endl;
    	usage(argv[0]);
    	exit(1);
    }
    else
    {
    	for(int i = 0; i < pcap_files.size(); i++)
    	{
    		std::ifstream tmp(pcap_files[i]);
    		if ( !tmp.good() )
    		{
    			LOG_ERROR(("Unable to find pcap file : %v", pcap_files[i]));
    			cerr << "ERROR: Unable to find pcap file : " << pcap_files[i] << endl;
    			exit(1);
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

	SLOG_INFO(<< "spid database : " << Config::instance()->spidDatabase());

	PcapReader reader;

	for(int i = 0; i < pcap_files.size(); i++)
	{
		if ( !reader.read(pcap_files[i]) )
		{
			SLOG_FATAL(<< "Unable to read pcap");
			exit(1);
		}
	}

}