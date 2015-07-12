/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_NETFILTER_PROCESS_H__
#define __VSID_NETFILTER_PROCESS_H__

#include <memory>
#include <mutex>
#include <vector>
#include <thread>

#include "PacketHandler.h"
#include "ProtocolModelDb.h"

namespace VsidNetfilter
{
/**
 * Main class to run the process from
 */
class Process
{
public:
	static Process* instance();
	virtual ~Process() {}

	/**
	 * Initialise the server
	 *
	 * @param  argc [description]
	 * @param  argv [description]
	 *
	 * @return
	 */
	bool initialise(int argc, char* argv[]);

	/**
	 * Run the main event loop
	 *
	 * @return
	 */
	bool run();

	/**
	 * Shutdown the process
	 */
	void shutdown();

	/**
     * Last signal received
     */
    static int sigReceived() { return _sigReceived; }
    static void sigReceived(int sig) { _sigReceived = sig; }

     /**
     * Set up all signals
     *
     * @param signal function
     */
    virtual void setUpSignals(void (*sig_fn)(int));

    /**
     * Print the program usage
     *
     * @param The name of the program i.e. argv[0]
     */
    virtual void usage(const char* command);

private:
	static int _sigReceived;

	static std::unique_ptr<Process> _instance;
	static std::once_flag _onceFlag;

	std::atomic<bool> _shutdown;

	Process() : _shutdown(false)
	{}
	
	Process(const Process& other) :
	_shutdown(false)
	{}
	
	Process& operator=(const Process& rhs) {}


	std::vector<std::shared_ptr<PacketHandler> > _packetHandlers;
	std::vector<std::thread> _handlerThreads;

	std::shared_ptr<Vsid::ProtocolModelDb> _protocolModelDb;
};


} // end namespace

#endif // END HEADER GUARD