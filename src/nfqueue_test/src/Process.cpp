/**
 * Main for netfilter program
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */


#include <stdio.h>
#include <getopt.h>
#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <ctime>

#include "Process.h"
#include "Logger.h"
#include "PacketHandler.h"
#include "Config.h"
#include "StringException.h"
#include "AttributeMeterRegistrar.h"
#include "CommonConfig.h"
 
using namespace std;
using namespace VsidNetfilter;
using namespace VsidCommon;
using namespace Vsid;

std::unique_ptr<Process> Process::_instance;
std::once_flag Process::_onceFlag;

int Process::_sigReceived = 0;


Process* Process::instance()
{
	std::call_once(_onceFlag,
        [] {
    if(!_instance) 
            _instance.reset(new Process);
    	}
    );

    return _instance.get();
}

// --------------------------------------------------------------------------
extern "C" void exitOnSignal(int sig)
{
    SLOG_INFO( << "exitOnSignal [" << sig << "]");
    Process::sigReceived(sig);
    Process::instance()->shutdown();
}

// --------------------------------------------------------------------------
void Process::setUpSignals(void (*sig_fn)(int) )
{
        struct sigaction action;

        // Set up signal handlers for sigintr/sigterm/sigquit to exit process cleanly
        action.sa_handler = sig_fn;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;

        int sig_err = sigaction(SIGINT, &action, NULL);
        if (sig_err)
        {
                SLOG_ERROR( << "ERROR: setting sig handler for SIGINT [" << errno << "] - ["
                        << strerror(errno) << "]" );
        }

        SLOG_INFO( << "Initialised signal handler for SIGINT");

        sig_err = sigaction(SIGTERM, &action, NULL);
        if (sig_err)
        {
                SLOG_ERROR( << "ERROR: setting sig handler for SIGTERM [" << errno << "] - ["
                        << strerror(errno) << "]" );
        }
        
        SLOG_INFO(<< "Initialised signal handler for SIGTERM");

        sig_err = sigaction(SIGQUIT, &action, NULL);
        if (sig_err)
        {
                SLOG_ERROR( << "ERROR: setting sig handler for SIGQUIT [" << errno << "] - ["
                        << strerror(errno) << "]" );
        }
        SLOG_INFO(<< "Initialised signal handler for SIGQUIT");

        return;
}


void Process::usage(const char* command)
{
	//cout << endl << gvProgramName <<  endl << endl;
	cout << "Usage : " << command << " [-h] [-c <config_file>]" << endl;
	cout << "Options :" << endl 
		<< " -c \t Config file " << endl
		<< " -l \t Logging Config " << endl
		<< " -h \t print this help and exit" << endl;
}


bool Process::initialise(int argc, char* argv[])
{
	setUpSignals(&exitOnSignal);


	string protocol_db;
	bool protocol_db_set = false;
	string config = "config.yaml";
	string training;

	char c;
	while ( (c = getopt(argc, argv, ":hc:d:") ) != -1) 
	{
	    switch (c) 
	    {
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

	SLOG_INFO(<< "config file : " << config);

	if ( !Config::instance()->init(config) )
	{
		SLOG_ERROR( << "Unable to initialise configuration");
		cerr << "ERROR: Unable to initialise configuration" << endl;
		exit(1);
	}

	return true;
}


bool Process::run()
{

	for(int i = 0; i < Config::instance()->numQueues(); i++ )
	{
		try
		{
			int offset = Config::instance()->queueOffset() + i;
			std::shared_ptr<PacketHandler> handler(new PacketHandler(offset));
			_packetHandlers.push_back(handler);

			_handlerThreads.push_back( std::thread(&PacketHandler::run, handler) );

		}
		catch(StringException& ex)
		{
			SLOG_ERROR( << "Exception : " << ex.what() )
			exit(1);
		}
	}
	
	std::time_t last = std::time(nullptr);
	std::time_t now = std::time(nullptr);

	while (!_shutdown)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		now = std::time(nullptr);
		if(now - last > 30)
		{
		    std::cout << "stats at: " << std::ctime(&now) << '\n';
			for(auto t : _packetHandlers){
				cout << "Queue [" << t->queueNumber() << "] received [" << t->numPackets() << "]" << endl;
                std::vector<uint64_t> vs = t->verdictStats();
                cout << "verdicts: " << endl;
                for(int i = 0; i < vs.size(); ++i)
                {
                    cout << "\t" << i << " = " << vs[i] << endl;
                }
                cout << "--------------------------" << endl;
			}
			cout << "--------------------------" << endl;
			last = now;
		}
	}

	//Join the threads with the main thread
	for(auto &t : _handlerThreads){
		t.join();
	}

	return true;
}

void Process::shutdown()
{
	SLOG_INFO(<< "Received shutdown command. Signal Received [" << _sigReceived << "]");
	_shutdown = true;

    std::time_t now = std::time(nullptr);
    std::cout << "stats before shutdown: " << std::ctime(&now) << '\n';
    for(auto &t : _packetHandlers) {
        cout << "Queue [" << t->queueNumber() << "] received [" << t->numPackets() << "]" << endl;
        std::vector<uint64_t> vs = t->verdictStats();
        cout << "verdicts: " << endl;
        for(int i = 0; i < vs.size(); ++i)
        {
            cout << "\t" << i << " = " << vs[i] << endl;
        }
        cout << "--------------------------" << endl;
    }

	for(auto &t : _packetHandlers){
		t->shutdown();
    }
    cout << endl << "--------------------------" << endl << endl;
    now = std::time(nullptr);
    std::cout << "stats at shutdown: " << std::ctime(&now) << '\n';
    for(auto &t : _packetHandlers) {
        cout << "Queue [" << t->queueNumber() << "] received [" << t->numPackets() << "]" << endl;
        std::vector<uint64_t> vs = t->verdictStats();
        cout << "verdicts: " << endl;
        for(int i = 0; i < vs.size(); ++i)
        {
            cout << "\t" << i << " = " << vs[i] << endl;
        }
        cout << "--------------------------" << endl;
	}
}
