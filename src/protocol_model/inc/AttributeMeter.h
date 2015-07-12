/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_METER_H__
#define __VSID_ATTRIBUTE_METER_H__

#include <string>
#include <vector>

#include "Flow.h"

namespace Vsid
{

class AttributeMeter
{
public:
	AttributeMeter(size_t fingerprint_size);
	virtual ~AttributeMeter() {}; 

	/**
	 * Unique name for the AttributeMeter
	 *
	 * @return
	 */
	virtual std::string name() const = 0;

	bool enabled() const { return _enabled; }
	uint32_t flowCount() const { return _flowCount; }

	const std::vector<double>& fingerprint() const { return _fingerprint; }

	virtual void calculateMeasurement(VsidCommon::Flow*  flow, 
													VsidCommon::IPv4Packet* currentPacket ) = 0;


	/**
	 * Get the first fingerprint
	 *
	 * @return
	 */
	double front();

	/**
	 * Get the fingerprint at position
	 *
	 * @param pos
	 * @return
	 */
	double at(size_t pos);

	/**
	 * Get number of fingerprint in the Meter
	 *
	 * @return
	 */
	size_t size() const { return _fingerprint.size(); }


	// TODO 
	// void update(std::shared_ptr<AttributeMeter> other)
	// 
	// 

	friend class ProtocolModel;
	friend class ProtocolModelDb;

protected:
	uint32_t _flowCount;
	std::vector<double> _fingerprint;
	bool _enabled;

	// used when reading from the DB to make sure that we have a correctly sized entry.
	size_t _fingerprint_size;

private:
	
};




} // end namespace

#endif // END HEADER GUARD