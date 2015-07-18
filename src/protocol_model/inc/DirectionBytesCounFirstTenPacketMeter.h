/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_DIRECTION_BYTES_COUNT_FIRST_TEN_METER_H__
#define __VSID_ATTRIBUTE_DIRECTION_BYTES_COUNT_FIRST_TEN_METER_H__

#include <string>
#include <vector>
#include <memory>
#include "AttributeMeterFactory.h"
#include "AttributeMeter.h"

namespace Vsid
{

/**
 * Average Bytes per direction of the first 10 packets.
 *
 * Should be good for protocols that have setup flows 
 *
 * Currently doesn't take into account flow setup with TCP. Hence the 10 packets 
 * i.e. 5 per direction. May overcompensate for UDP flows where the setup
 * handshake may only be a 3 or 4 way handshake
 * 
 */
class DirectionBytesCounFirstTenPacketMeter : public AttributeMeter
{
public:
	DirectionBytesCounFirstTenPacketMeter();
	virtual ~DirectionBytesCounFirstTenPacketMeter() {}; 

	/**
	 * Unique name for the AttributeMeter
	 *
	 * @return
	 */
	virtual std::string name() const { return "DirectionBytesCounFirstTenPacketMeter"; }

	
	virtual void calculateMeasurement(VsidCommon::Flow*  flow, 
													VsidCommon::IPv4Packet* currentPacket );

	// TODO 
	// void update(std::shared_ptr<AttributeMeter> other)
	// 
	// 

	friend class ProtocolModel;
	friend class ProtocolModelDb;

	static Vsid::Registrar registrar;
protected:
	uint64_t _overall_byte_size;
private:

};

} // end namespace

#endif // END HEADER GUARD