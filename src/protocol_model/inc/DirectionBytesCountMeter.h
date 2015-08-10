/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_DIRECTION_BYTES_COUNT_METER_H__
#define __VSID_ATTRIBUTE_DIRECTION_BYTES_COUNT_METER_H__

#include <string>
#include <vector>
#include <memory>
#include "AttributeMeterFactory.h"
#include "AttributeMeter.h"

namespace Vsid
{

/**
 * The direction bytes count meter analyses the total byte count in each direction of the flow. 
 * This is for all data packets in all directions up to the defining limit. 
 * 
 * This attribute can be useful for defining if a protocol is used for 
 * bulk data or request / response pairs. In bulk data protocols such as RTMP, 
 * most data will be in a single direction, however for request response protocols such as RTSP, 
 * the data exchanged in each direction should be similar. 
 */
class DirectionBytesCountMeter : public AttributeMeter
{
public:
	DirectionBytesCountMeter();
	virtual ~DirectionBytesCountMeter() {}; 

	/**
	 * Unique name for the AttributeMeter
	 *
	 * @return
	 */
	virtual std::string name() const { return "DirectionBytesCountMeter"; }

	
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