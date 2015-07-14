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
class FirstPacketBytesMeter : public AttributeMeter
{
public:
	FirstPacketBytesMeter();
	virtual ~FirstPacketBytesMeter() {}; 

	/**
	 * Unique name for the AttributeMeter
	 *
	 * @return
	 */
	virtual std::string name() const { return "FirstPacketBytesMeter"; }

	
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