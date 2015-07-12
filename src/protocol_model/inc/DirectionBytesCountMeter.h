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

private:

};

} // end namespace

#endif // END HEADER GUARD