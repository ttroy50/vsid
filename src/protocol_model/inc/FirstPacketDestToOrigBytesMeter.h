/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_FIRST_PACKET_BYTE_DESTORIG_METER_H__
#define __VSID_ATTRIBUTE_FIRST_PACKET_BYTE_DESTORIG_METER_H__

#include <string>
#include <vector>
 
#include "AttributeMeter.h"
#include "AttributeMeterFactory.h"

namespace Vsid
{

/**
 * Byte Frequency for first non empty DEST to ORIG packet
 * 
 */
class FirstPacketDestToOrigBytesMeter : public AttributeMeter
{
public:
    FirstPacketDestToOrigBytesMeter();
    virtual ~FirstPacketDestToOrigBytesMeter() {}; 

    /**
     * Unique name for the AttributeMeter
     *
     * @return
     */
    virtual std::string name() const { return "FirstPacketDestToOrigBytesMeter"; }

    
    virtual void calculateMeasurement(VsidCommon::Flow* flow, 
                                                    VsidCommon::IPv4Packet* currentPacket );

    // TODO 
    // virtual void update(std::vector<double>)
    // 
    // 

    friend class ProtocolModel;
    friend class ProtocolModelDb;

    static Vsid::Registrar registrar;
protected:

private:
    bool _done;
};




} // end namespace

#endif // END HEADER GUARD