/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_FLOW_OBSERVERS_H__
#define __VSID_FLOW_OBSERVERS_H__


#include "Flow.h"
namespace VsidCommon
{

/**
 * Called when a flow has finished and is being deleted
 */
class FlowFinishedObserver
{
public:
    virtual void flowFinished(std::shared_ptr<Flow> flow) = 0;
};

/**
 * Called when a flow has been classified
 */
class FlowClassifiedObserver
{
public:
    virtual void flowClassified(std::shared_ptr<Flow> flow) = 0;
};


} // end namespace

#endif // END HEADER GUARD