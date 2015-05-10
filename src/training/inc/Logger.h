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
#define SLOG_WARN(X) LOG(INFO) X;
#define SLOG_ERROR(X) LOG(INFO) X;
#define SLOG_DEBUG(X) LOG(INFO) X;
#define SLOG_FATAL(X) LOG(INFO) X;
#define SLOG_TRACE(X) LOG(INFO) X;

#endif 