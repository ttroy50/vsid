/**
 * Logging. 
 * Just wrappers around easylogging++ in case I decide to change later
 *
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef __VSID_COMMON_LOGGER_H__
#define __VSID_COMMON_LOGGER_H__

#define ELPP_NO_DEFAULT_LOG_FILE

#include "easylogging++.h"

#define INIT_LOGGING INITIALIZE_EASYLOGGINGPP

#define ELPP_DISABLE_DEFAULT_CRASH_HANDLING
//#define ELPP_STACKTRACE_ON_CRASH



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
#define SLOG_WARN(X) LOG(WARNING) X;
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
inline int format_hexdump (const u_char *buffer, int size,
        char *obuf, int obuf_sz)
{
    u_char c;
    char textver[16 + 1];

    //int maxlen = (obuf_sz / 68) * 16;
    int maxlen = (obuf_sz / 69) * 16; // +1 for \t

    if (size > maxlen)
        size = maxlen;

    int i;

    for (i = 0; i < (size >> 4); i++)
    {
        int j;

        *obuf++ = '\t';
        for (j = 0 ; j < 16; j++)
        {
            c = (u_char) buffer[(i << 4) + j];
            ::sprintf (obuf, "%02X ", c);
            obuf += 3;
            if (j == 7)
            {
                ::sprintf (obuf, " ");
                obuf++;
            }
            textver[j] = (c < 0x20 || c > 0x7e) ? '.' : c;
        }

        textver[j] = 0;

        ::sprintf (obuf, "  %s\n", textver);

        while (*obuf != '\0')
            obuf++;
    }

    if (size % 16)
    {
        *obuf++ = '\t';
        for (i = 0 ; i < size % 16; i++)
        {
            c = (u_char) buffer[size - size % 16 + i];
            ::sprintf (obuf, "%02X ",c);
            obuf += 3;
            if (i == 7)
            {
                ::sprintf (obuf, " ");
                obuf++;
            }
            textver[i] = (c < 0x20 || c > 0x7e) ? '.' : c;
        }

        for (i = size % 16; i < 16; i++)
        {
            ::sprintf (obuf, "   ");
            obuf += 3;
            textver[i] = ' ';
        }

        textver[i] = 0;
        ::sprintf (obuf, "  %s\n", textver);
    }
    return size;
}

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
 		char LHD_buf[4097]; 	\
 		LHD_buf[0] = '\0'; 		\
 		int len = format_hexdump (HEX, SZE, LHD_buf, sizeof LHD_buf); \
 		LOG(INFO) << MSG << endl << LHD_buf << endl; \
 	}


#define CLASS_LOG(OutputStreamInstance) \
 	virtual void log(el::base::type::ostream_t& OutputStreamInstance) const 


 	
#endif // end HEADER GUARD