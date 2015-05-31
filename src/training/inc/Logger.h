/**
 * Logging. 
 * Just wrappers around easylogging++ in case I decide to change later
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_TRAINING_LOGGER_H__
#define __VSID_TRAINING_LOGGER_H__

#define ELPP_NO_DEFAULT_LOG_FILE

#include "easylogging++.h"

#define INIT_LOGGING INITIALIZE_EASYLOGGINGPP 


// printf style wrappers
// call like LOG_INFO(("some variable %v", variable))
#define LOG_INFO(X) el::Loggers::getLogger("default")->info X;
#define LOG_WARN(X) el::Loggers::getLogger("default")->warn X;
#define LOG_ERROR(X) el::Loggers::getLogger("default")->error X;
#define LOG_DEBUG(X) el::Loggers::getLogger("default")->debug X;
#define LOG_FATAL(X) el::Loggers::getLogger("default")->fatal X;
#define LOG_TRACE(X) el::Loggers::getLogger("default")->trace X;

// wrappers around stream logger
// call lile SLOG_INFO(<< "some variable " << varlaboe)
#define SLOG_INFO(X) LOG(INFO) X;
#define SLOG_WARN(X) LOG(WARN) X;
#define SLOG_ERROR(X) LOG(ERROR) X;
#define SLOG_DEBUG(X) LOG(DEBUG) X;
#define SLOG_FATAL(X) LOG(FATAL) X;
#define SLOG_TRACE(X) LOG(TRACE) X;


/**
 * Format a buffer to print it as hex. The resulting outbuf looks like
 *
 * 4C 60 DE D8 D8 FA 08 3E  8E 64 AE E0 08 00 45 00   L`.....>.d....E.
 * 
 * @param  buffer Max buffer size is 1000
 * @param  size size of buffer
 * @param  obuf
 * @param  obuf_sz returned size of output buffer
 * @return
 */
int format_hexdump(const u_char *buffer, int size, char *obuf, int obuf_sz);

/**
 * Log a buffer as HEX. 
 * 
 * Mainly for logging a packet 
 *
 * Use sparingly
 * @param  MSG A msg to print in the first line
 * @param  HEX The hex buffer
 * @param  SZE Size of the hex buffer
 * @return
 */
#define LOG_HEXDUMP(MSG, HEX, SZE) \
 	if( SZE < 2048) { \
 		char buf[4097]; 	\
 		buf[0] = '\0'; 		\
 		int len = format_hexdump (HEX, SZE, buf, sizeof buf); \
 		LOG(INFO) << MSG << endl << buf << endl; \
 	}

#endif 