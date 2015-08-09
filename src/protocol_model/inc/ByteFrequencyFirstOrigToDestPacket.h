/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_FIRST_PACKET_BYTE_METER_H__
#define __VSID_ATTRIBUTE_FIRST_PACKET_BYTE_METER_H__

#include <string>
#include <vector>
 
#include "AttributeMeter.h"
#include "AttributeMeterFactory.h"

namespace Vsid
{

/**
 * Byte Frequency for first ORIG to DEST packet
 * 
 */
class ByteFrequencyFirstOrigToDestPacket : public AttributeMeter
{
public:
	ByteFrequencyFirstOrigToDestPacket();
	virtual ~ByteFrequencyFirstOrigToDestPacket() {}; 

	/**
	 * Unique name for the AttributeMeter
	 *
	 * @return
	 */
	virtual std::string name() const { return "ByteFrequencyFirstOrigToDestPacket"; }

	
	virtual void calculateMeasurement(VsidCommon::Flow* flow, 
													VsidCommon::IPv4Packet* currentPacket );

	virtual double at(size_t pos);


	friend class ProtocolModel;
	friend class ProtocolModelDb;

	static Vsid::Registrar registrar;
protected:
	uint64_t _overall_byte_size;
private:

};




} // end namespace

#endif // END HEADER GUARD