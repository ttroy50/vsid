/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_TRAINING_CONSTANTS_H__
#define __VSID_TRAINING_CONSTANTS_H__

namespace VSID_TRAINING
{

	// LINK LAYER HEADER SIZES
	static const int LL_ETH_HDR_LEN = 14;
	static const int LL_SLL_HDR_LEN = 16; // = SLL_HDR_LEN from pcap/sll.h

	// IP version numbers
	static const int IPv4 = 4;
	static const int IPv6 = 6;

	
} // end namespace

#endif // END HEADER GUARD