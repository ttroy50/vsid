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

 
	void merge(std::shared_ptr<AttributeMeter> other);

	/**
	 * These are used to introduce noise into the K-L divergence to 
	 * prevent divide by 0 issues
	 *
	 * The reason is that for a K-L divergence to be valid it must satisfy 2 things
	 * Sum of all P(i) = 1
	 * Sum of all Q(i) = 1
	 * P(i) or Q(i) cannot be zero when the other has a value
	 * @return
	 */
	double klFixMultiplier() const { return _klFixMultiplicator; }
	double klFixIncrement() const { return _klFixIncrement; }

	friend class ProtocolModel;
	friend class ProtocolModelDb;

protected:
	uint32_t _flowCount;
	std::vector<double> _fingerprint;
	bool _enabled;

	// used when reading from the DB to make sure that we have a correctly sized entry.
	size_t _fingerprint_size;

	double _klFixMultiplicator;
	double _klFixIncrement;
private:
	
};




} // end namespace

#endif // END HEADER GUARD