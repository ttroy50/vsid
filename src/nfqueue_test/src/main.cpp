/**
 * Main for netfilter program
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */


#include <stdio.h>
#include <getopt.h>

#include "Process.h"
#include "Logger.h"
 
using namespace std;
using namespace VsidNetfilter;

static const char* const gvProgramName = "VSID - Netfilter Program";

INIT_LOGGING

int main( int argc, char* argv[] )
{
	// TODO read from argv	
	el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
	el::Configurations conf("../config/logging_netfilter.conf"); 
	el::Loggers::reconfigureAllLoggers(conf);

	SLOG_INFO(	<< endl 
				<< "\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+" << endl
	   			<< "\t\t+ Running " << argv[0] << endl
				<< "\t\t+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+" << endl);

	try
	{

		if( !Process::instance()->initialise(argc, argv) )
		{
			SLOG_ERROR( << "Unable to initialise process");
			exit(1);
		}

		Process::instance()->run();
	}
	catch(exception& e)
	{
		SLOG_ERROR(<< "Caught exception [ " << e.what() << " ]");
		exit(1);
	}

	SLOG_INFO(<< "Finished program")
}