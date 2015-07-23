/**
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_ATTRIBUTE_METER_REGISTRAR_H__
#define __VSID_ATTRIBUTE_METER_REGISTRAR_H__

#include "Logger.h"

#include "AttributeMeter.h"
#include "ByteFrequency.h"
#include "DirectionBytesCountMeter.h"
#include "FirstPacketBytesMeter.h"
#include "FirstFourByteFrequencyMeter.h"
#include "FirstSixteenByteFrequencyMeter.h"
#include "DirectionChangesMeter.h"
#include "FirstPacketDestToOrigBytesMeter.h"
#include "DirectionBytesCounFirstTenPacketMeter.h"
#include "ByteFrequencyFirstEightPacketsMeter.h"
#include "DirectionChangesFirstEightPacketsMeter.h"
#include "RtmpRegexMatchMeter.h"

/**
 * Make sure to call this at the start of your program
 */
inline void init_attribute_meters()
{
	SLOG_INFO(<< "Initialising Attribute Meters");
	Vsid::DirectionBytesCountMeter::registrar.doRegistration();
	Vsid::ByteFrequencyMeter::registrar.doRegistration();
	Vsid::FirstPacketBytesMeter::registrar.doRegistration();
    Vsid::FirstFourByteFrequencyMeter::registrar.doRegistration();
    Vsid::FirstSixteenByteFrequencyMeter::registrar.doRegistration();
    Vsid::DirectionChangesMeter::registrar.doRegistration();
    Vsid::FirstPacketDestToOrigBytesMeter::registrar.doRegistration();
    Vsid::DirectionBytesCounFirstTenPacketMeter::registrar.doRegistration();
    Vsid::ByteFrequencyFirstEightPacketsMeter::registrar.doRegistration();
    Vsid::DirectionChangesFirstEightPacketsMeter::registrar.doRegistration();
    Vsid::RtmpRegexMatchMeter::registrar.doRegistration();
	SLOG_INFO(<< "Attribute Meters Initialised");
} 

#endif // END HEADER GUARD