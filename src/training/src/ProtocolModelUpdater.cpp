#include "ProtocolModelUpdater.h"
#include "FlowManager.h"
#include "ProtocolModelDb.h"

using namespace VsidTraining;
using namespace Vsid;
using namespace VsidCommon;
using namespace std;

ProtocolModelUpdater::ProtocolModelUpdater(FlowManager* manager, ProtocolModelDb* database) :
    _currentFile(NULL),
    _updated(false)
{
    _manager = manager;
    _database = database;
    _manager->addFlowFinishedObserver(this);
}

void ProtocolModelUpdater::flowFinished(std::shared_ptr<Flow> flow)
{
    SLOG_INFO(<< "Flow [" << flow->flowHash() 
                    //<< "] finished in state [" << flow->flowState() 
                    << "] with [" << flow->pktCount() << "] packets");

    if(_currentFile == NULL)
    {
        SLOG_ERROR(<< "Current file is empty");
    }

    bool flowFound = false;
    for(auto it = _currentFile->flows.begin(); it != _currentFile->flows.end(); ++it)
    {
        if(it->flowHash() == flow->flowHash())
        {
            SLOG_INFO(<< "Found flow in training file " << it->tuple)
            flowFound = true;
            auto meters = flow->attributeMeters();
            std::shared_ptr<ProtocolModel> model = _database->find(it->protocol);

            if ( !model )
            {
                SLOG_ERROR(<< "Unable to find model : " << it->protocol);
                break;
            }

            for(auto meterIt = meters.begin(); meterIt != meters.end(); ++meterIt)
            {
                auto modelMeter = model->find((*meterIt)->name());
                if(modelMeter)
                {
                    _updated = true;
                    modelMeter->merge((*meterIt));
                }
                else
                {
                    SLOG_ERROR(<< "Unable to find meter [" << (*meterIt)->name() 
                                    << "] in model [" << model->name() << "]");
                }
            }
            break;
        }
        else
        {
            //SLOG_ERROR(<< "Don't match " << *flow << "\n" << it->tuple);
            //SLOG_ERROR(<< "Don't match : ffh : " << flow->flowHash() << " : itfh : " << it->flowHash());
        }
    }

    if(!flowFound)
    {
        SLOG_ERROR(<< "Unable to find flow in current training file : " << *flow );
    }
}