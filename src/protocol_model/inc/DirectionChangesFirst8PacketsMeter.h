/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_DIRECTION_CHANGES__FIRST_8_METER_H__
#define __VSID_ATTRIBUTE_DIRECTION_CHANGES__FIRST_8_METER_H__

#include <string>
#include <vector>
 
#include "AttributeMeter.h"
#include "AttributeMeterFactory.h"

namespace Vsid
{

/**
 * Frequency of direction changes
 * 
 */
class DirectionChangesFirst8PacketsMeter : public AttributeMeter
{
public:
    DirectionChangesFirst8PacketsMeter();
    virtual ~DirectionChangesFirst8PacketsMeter() {}; 

    /**
     * Unique name for the AttributeMeter
     *
     * @return
     */
    virtual std::string name() const { return "DirectionChangesFirst8PacketsMeter"; }

    
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

private:

};




} // end namespace

#endif // END HEADER GUARD