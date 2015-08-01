/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_ENTROPY_FIRST_ORIG_TO_DEST_H__
#define __VSID_ATTRIBUTE_ENTROPY_FIRST_ORIG_TO_DEST_H__

#include <string>
#include <vector>
 
#include "AttributeMeter.h"
#include "AttributeMeterFactory.h"

namespace Vsid
{

/**
 * Entropy for first ORIG to DEST packet
 * 
 */
class EntropyFirstOrigToDestPacket : public AttributeMeter
{
public:
    EntropyFirstOrigToDestPacket();
    virtual ~EntropyFirstOrigToDestPacket() {}; 

    /**
     * Unique name for the AttributeMeter
     *
     * @return
     */
    virtual std::string name() const { return "EntropyFirstOrigToDestPacket"; }

    
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

};




} // end namespace

#endif // END HEADER GUARD