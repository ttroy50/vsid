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
 
namespace Vsid
{

class AttributeMeter
{
public:
	AttributeMeter();
	virtual ~AttributeMeter() {}; 

	std::string name() const { return _name; }
	bool enabled() const { return _enabled; }
	uint32_t flowCount() const { return _flowCount; }

	const std::vector<double>& fingerprint() const { return _fingerprint; }

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

private:
	std::string _name;
	bool _enabled;
	
};




} // end namespace

#endif // END HEADER GUARD