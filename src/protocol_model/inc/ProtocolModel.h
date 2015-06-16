/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_PROTOCOL_MODEL_H__
#define __VSID_PROTOCOL_MODEL_H__

#include <string>
#include <vector>
#include <memory>

#include "AttributeMeter.h"
 
namespace Vsid
{


class ProtocolModel
{
public:
	ProtocolModel();
	virtual ~ProtocolModel() {};

	const std::string& name() const { return _name; }
	uint32_t flowCount() const { return _flowCount; }
	bool enabled() const { return _enabled; }
	const std::vector<uint16_t>& portHints() const { return _portHints; }

	/**
	 * Find a AttributeMeter by name
	 *
	 * @param name
	 * @return
	 */
	std::shared_ptr<AttributeMeter> find(const std::string& name);

	/**
	 * Get the first attribute meter
	 *
	 * @return
	 */
	std::shared_ptr<AttributeMeter> front();

	/**
	 * Get the AttributeMeter at position
	 *
	 * @param pos
	 * @return
	 */
	std::shared_ptr<AttributeMeter> at(size_t pos);

	/**
	 * Get number of attribute meters in the list
	 *
	 * @return
	 */
	size_t size() const { return _attributeMeters.size(); }

	friend class ProtocolModelDb;

private:
	std::string _name;
	uint32_t _flowCount;
	bool _enabled;

	std::vector<uint16_t> _portHints;
	typedef std::vector<std::shared_ptr<AttributeMeter> > AttributeMeterVector;
	AttributeMeterVector _attributeMeters;
};



} // end namespace

#endif // END HEADER GUARD