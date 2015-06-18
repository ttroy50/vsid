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