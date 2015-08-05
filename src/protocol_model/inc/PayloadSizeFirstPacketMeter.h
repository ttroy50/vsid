/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_FIRST_PAYLAOD_SIZE_H__
#define __VSID_ATTRIBUTE_FIRST_PAYLAOD_SIZE_H__

#include <string>
#include <vector>
 
#include "AttributeMeter.h"
#include "AttributeMeterFactory.h"

namespace Vsid
{

/**
 * Byte Frequency for all packets.
 * 
 */
class PayloadSizeFirstPacketMeter : public AttributeMeter
{
public:
    PayloadSizeFirstPacketMeter();
    virtual ~PayloadSizeFirstPacketMeter() {}; 

    /**
     * Unique name for the AttributeMeter
     *
     * @return
     */
    virtual std::string name() const { return "PayloadSizeFirstPacketMeter"; }

    
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

