/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_PCAP_CLASSIFIED_LOGGER_H__
#define __VSID_PCAP_CLASSIFIED_LOGGER_H__

#include "Flow.h"
#include "FlowObservers.h"

namespace VsidCommon
{

class FlowManager;

/**
 * Config Singleton
 */
class FlowClassificationLogger : 
    public VsidCommon::FlowFinishedObserver, 
    public VsidCommon::FlowClassifiedObserver
{
public:
    FlowClassificationLogger(VsidCommon::FlowManager* manager);
    ~FlowClassificationLogger() {}

    /**
     * A Flow has finished. Lets update the protocol model DB
     * @param flow
     */
    virtual void flowFinished(std::shared_ptr<VsidCommon::Flow> flow);


    virtual void flowClassified(std::shared_ptr<VsidCommon::Flow> flow); 


private:
    VsidCommon::FlowManager* _manager;

};


} // end namespace

#endif // END HEADER GUARD