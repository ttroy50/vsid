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
#include "ByteFrequencyFirstOrigToDestPacket.h"
#include "First4ByteFrequencyMeter.h"
#include "First16ByteFrequencyMeter.h"
#include "DirectionChangesMeter.h"
#include "ByteFrequencyFirstDestToOrigPacket.h"
#include "DirectionBytesCountFirst10PacketMeter.h"
#include "ByteFrequencyFirst8PacketsMeter.h"
#include "DirectionChangesFirst8PacketsMeter.h"
#include "RtmpRegexMatchMeter.h"
#include "EntropyFirstOrigToDestPacket.h"
#include "ActionReactionFirst3ByteHashMeter.h"
#include "PayloadSizeFirstPacketMeter.h"
#include "FirstBitPositionsMeter.h"

/**
 * Make sure to call this at the start of your program
 */
inline void init_attribute_meters()
{
	SLOG_INFO(<< "Initialising Attribute Meters");
	Vsid::DirectionBytesCountMeter::registrar.doRegistration();
	Vsid::ByteFrequencyMeter::registrar.doRegistration();
	Vsid::ByteFrequencyFirstOrigToDestPacket::registrar.doRegistration();
    Vsid::First4ByteFrequencyMeter::registrar.doRegistration();
    Vsid::First16ByteFrequencyMeter::registrar.doRegistration();
    Vsid::DirectionChangesMeter::registrar.doRegistration();
    Vsid::ByteFrequencyFirstDestToOrigPacket::registrar.doRegistration();
    Vsid::DirectionBytesCountFirst10PacketMeter::registrar.doRegistration();
    Vsid::ByteFrequencyFirst8PacketsMeter::registrar.doRegistration();
    Vsid::DirectionChangesFirst8PacketsMeter::registrar.doRegistration();
    Vsid::RtmpRegexMatchMeter::registrar.doRegistration();
    Vsid::EntropyFirstOrigToDestPacket::registrar.doRegistration();
    Vsid::ActionReactionFirst3ByteHashMeter::registrar.doRegistration();
    Vsid::PayloadSizeFirstPacketMeter::registrar.doRegistration();
    Vsid::FirstBitPositionsMeter::registrar.doRegistration();
	SLOG_INFO(<< "Attribute Meters Initialised");
} 

#endif // END HEADER GUARD