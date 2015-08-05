/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_BIT_POSITION_FREQUENCY_H__
#define __VSID_ATTRIBUTE_BIT_POSITION_FREQUENCY_H__

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
class FirstBitPositionsMeter : public AttributeMeter
{
public:
    FirstBitPositionsMeter();
    virtual ~FirstBitPositionsMeter() {}; 

    /**
     * Unique name for the AttributeMeter
     *
     * @return
     */
    virtual std::string name() const { return "FirstBitPositionsMeter"; }

    
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

    uint64_t _overall_bit_size;
private:

};




} // end namespace

#endif // END HEADER GUARD