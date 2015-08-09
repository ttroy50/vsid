/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_FIRST_FOUR_BYTE_FREQUENCY_H__
#define __VSID_ATTRIBUTE_FIRST_FOUR_BYTE_FREQUENCY_H__

#include <string>
#include <vector>
 
#include "AttributeMeter.h"
#include "AttributeMeterFactory.h"

namespace Vsid
{

/**
 * Byte Frequency for first 4 bytes of all packets.
 * 
 */
class First4ByteFrequencyMeter : public AttributeMeter
{
public:
    First4ByteFrequencyMeter();
    virtual ~First4ByteFrequencyMeter() {}; 

    /**
     * Unique name for the AttributeMeter
     *
     * @return
     */
    virtual std::string name() const { return "First4ByteFrequencyMeter"; }

    virtual double at(size_t pos);

    virtual void calculateMeasurement(VsidCommon::Flow* flow, 
                                                    VsidCommon::IPv4Packet* currentPacket );

    friend class ProtocolModel;
    friend class ProtocolModelDb;

    static Vsid::Registrar registrar;
protected:

private:
    uint64_t _overall_byte_size;

};




} // end namespace

#endif // END HEADER GUARD