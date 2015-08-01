/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_ACTION_REACTION_FIRST_3_BYTE_METER_H__
#define __VSID_ATTRIBUTE_ACTION_REACTION_FIRST_3_BYTE_METER_H__

#include <string>
#include <vector>
 
#include "AttributeMeter.h"
#include "AttributeMeterFactory.h"

#define NUM_AR_BYTES 3
#define MAX_AR_CS 114

namespace Vsid
{

/**
 * Frequency of direction changes
 * 
 */
class ActionReactionFirst3ByteHashMeter : public AttributeMeter
{
public:
    ActionReactionFirst3ByteHashMeter();
    virtual ~ActionReactionFirst3ByteHashMeter() {}; 

    /**
     * Unique name for the AttributeMeter
     *
     * @return
     */
    virtual std::string name() const { return "ActionReactionFirst3ByteHashMeter"; }

    
    virtual void calculateMeasurement(VsidCommon::Flow* flow, 
                                                    VsidCommon::IPv4Packet* currentPacket );

    // TODO 
    // void update(std::shared_ptr<AttributeMeter> other)
    // 
    // 

    friend class ProtocolModel;
    friend class ProtocolModelDb;

    static Vsid::Registrar registrar;
protected:
    u_char _lastPacket[NUM_AR_BYTES];
    size_t _lastPacketSize;
    size_t _overall_reaction_num;

private:

};




} // end namespace

#endif // END HEADER GUARD