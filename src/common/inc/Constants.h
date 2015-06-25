/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_COMMON_CONSTANTS_H__
#define __VSID_COMMON_CONSTANTS_H__

namespace VsidCommon
{

	// LINK LAYER HEADER SIZES
	static const int LL_ETH_HDR_LEN = 14;
	static const int LL_SLL_HDR_LEN = 16; // = SLL_HDR_LEN from pcap/sll.h

	// IP version numbers
	static const int IPv4 = 4;
	static const int IPv6 = 6;

	
} // end namespace

#endif // END HEADER GUARD