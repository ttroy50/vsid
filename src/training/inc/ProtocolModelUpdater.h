/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_TRAINING_MODEL_UPDATER_H__
#define __VSID_TRAINING_MODEL_UPDATER_H__

#include "Flow.h"
#include "FlowObservers.h"
#include "TrainingInput.h"

namespace VsidCommon
{
    class FlowManager;
}

namespace Vsid
{
    class ProtocolModelDb;
}
namespace VsidTraining
{

/**
 * Config Singleton
 */
class ProtocolModelUpdater : public VsidCommon::FlowFinishedObserver
{
public:
    ProtocolModelUpdater(VsidCommon::FlowManager* manager, Vsid::ProtocolModelDb* database);
    ~ProtocolModelUpdater() {}

    /**
     * The current file we are working on so we can find the flow.
     * Not thread safe but ok for now
     * @param file
     */
    void setCurrentFile(TrainingFile* file) { _currentFile = file; }

    /**
     * A Flow has finished. Lets update the protocol model DB
     * @param flow
     */
    virtual void flowFinished(std::shared_ptr<VsidCommon::Flow> flow);

    bool updated() { return _updated; }

private:
    VsidCommon::FlowManager* _manager;
    Vsid::ProtocolModelDb* _database;

    TrainingFile* _currentFile;
    bool _updated;
};


} // end namespace

#endif // END HEADER GUARD