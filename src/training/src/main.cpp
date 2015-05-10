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

#include "Logger.h"

using namespace std;

static const char* const gvProgramName = "VSI SPID Training Program";

INIT_LOGGING

void usage(const char* command)
{
	cout << gvProgramName <<  endl << endl;
	cout << "Usage : " << command << " [-h] [-c <config_file>]";
	cout << "Options :" << endl 
		<< "-c --config "
		<< "-h : print this help" << endl;

}


int main( int argc, char* argv[] )
{

	string config = "config.yaml";
	char c;
	while ( (c = getopt(argc, argv, "hc:") ) != -1) 
	{
	    switch (c) 
	    {
	        case 'c':
	            config = optarg;
	            if(config.size() == 0)
	            {
	            	cout << "Config cannot be empty" << endl;
	            	usage(argv[0]);
	            	exit(1);
	            }
	            break;
	        case 'h':
	        	usage(argv[0]);
	        	exit(0);
	        default:
	            cout << "Unrecognised Option" << endl;
	            usage(argv[0]);
	            exit(1);
	            break;
	    }
	}

	LOG(INFO) << "config file " << config;
}