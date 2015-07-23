/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_RTMP_REGEX_MATCH_H__
#define __VSID_ATTRIBUTE_RTMP_REGEX_MATCH_H__

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
class RtmpRegexMatchMeter : public AttributeMeter
{
public:
    RtmpRegexMatchMeter();
    virtual ~RtmpRegexMatchMeter() {}; 

    /**
     * Unique name for the AttributeMeter
     *
     * @return
     */
    virtual std::string name() const { return "RtmpRegexMatchMeter"; }

    
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
    uint64_t _overall_byte_size;

};




} // end namespace

#endif // END HEADER GUARD