#include "FlowClassificationLogger.h"
#include "FlowManager.h"
#include "Logger.h"

using namespace Vsid;
using namespace VsidCommon;
using namespace std;

FlowClassificationLogger::FlowClassificationLogger(FlowManager* manager)
{
    _manager = manager;
    _manager->addFlowFinishedObserver(this);
    _manager->addFlowClassifiedObserver(this);
}

void FlowClassificationLogger::flowFinished(std::shared_ptr<Flow> flow)
{
    if( !flow->flowClassified() )
    {
        // In YAML to allow for easy parsing
        SLOG_TRACE(<< "  - { Result: NotClassified"
                    << ", BestMatch: " << flow->bestMatchProtocol() 
                    << ", BestMatchDivergence: " << flow->bestMatchDivergence() 
                    << ", Flow: " << *flow
                    << "}");
    }

}

void FlowClassificationLogger::flowClassified(std::shared_ptr<Flow> flow)
{
    // In YAML to allow for easy parsing
    SLOG_TRACE(<< "  - { Result: Classified"
                << ", Protocol: " << flow->classifiedProtocol() 
                << ", Divergence: " << flow->classifiedDivergence() 
                << ", Flow: " << *flow
                << "}");
}