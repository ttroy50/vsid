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
	        	LOG(INFO) << "-d " << optarg;
	            spid_db = optarg;
	            break;
	        case 'c':
	        	LOG(INFO) << "-c " << optarg;
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
        LOG(INFO) << (argc - optind) << " pcap file(s) : ";
        while (optind < argc)
        {
        	pcap_files.push_back(argv[optind++]);
        	LOG(INFO) << "\t" << pcap_files.back();
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
    			LOG(ERROR) << "Unable to find pcap file : " << pcap_files[i];
    			cerr << "ERRO: Unable to find pcap file : " << pcap_files[i] << endl;
    			exit(1);
    		}
    	}
    }

    LOG(INFO) << "config file : " << config;

	if ( !Config::instance()->init(config) )
	{
		LOG(ERROR) << "Unable to initialise configuration";
		cerr << "ERROR: Unable to initialise configuration" << endl;
		exit(1);
	}

	if( !spid_db.empty() )
	{
		LOG(INFO) << "overriding spid_db from config : " << spid_db; 
		Config::instance()->spidDatabase(spid_db);
	}

	LOG(INFO) << "spid database : " << Config::instance()->spidDatabase();

}