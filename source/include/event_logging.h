#ifndef __EVEMT_LOGGING_H__
#define __EVEMT_LOGGING_H__
#include <event_config.h>

#ifdef __cplusplus
// if you need to close the system printout, please use the macro "GOOGLE_LOGGING" to close it
#   ifndef GOOGLE_GLOG_DLL_DECL
#       define GOOGLE_GLOG_DLL_DECL
#   endif

#   ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#       define GLOG_NO_ABBREVIATED_SEVERITIES
#   endif

#include <logging.h>

#ifdef GOOGLE_LOGGING
#       define EVENT_CANCEL_LOGGING 1
#       define EVENT_SYSTEM_LOGGING 1
#endif // GOOGLE_LOGGING

#if (defined(_DEBUG) || defined(DEBUG) || defined(_DEBUG_))
#       ifndef GOOGLE_LOGGING
#           define GOOGLE_LOG(severity)                             LOG(severity) 
#           define                                                  DebugMsg(severity, ...) { String debugMsg[1024]; _snprintf_s(debugMsg, std::size(debugMsg), _TRUNCATE, ##__VA_ARGS__); google::LogMessage(__FILE__, __LINE__, google::GLOG_##severity).stream() << debugMsg; };
#           define GOOGLE_LOG_DEBUG_MSG(severity, fmt)              DebugMsg(severity, fmt)
#       else
#           define GOOGLE_LOG(severity)                       if(0) LOG(severity)
#           define                                                  DebugMsg(severity, ...) {};
#           define GOOGLE_LOG_DEBUG_MSG(severity, fmt)        if(0) DebugMsg(severity, fmt)
#       endif
#else
#           define GOOGLE_LOG(severity)                       if(0) LOG(severity)
#           define                                                  DebugMsg(severity, ...) {};
#           define GOOGLE_LOG_DEBUG_MSG(severity, fmt)        if(0) DebugMsg(severity, fmt)
#endif

#ifndef GOOGLE_LOGGING
#       define LOG_INFO                                             GOOGLE_LOG(INFO)
#       define LOG_WARN                                             GOOGLE_LOG(WARNING)
#       define LOG_ERROR                                            GOOGLE_LOG(ERROR)
#       define LOG_FATAL                                            GOOGLE_LOG(FATAL)
#elif   EVENT_CANCEL_LOGGING
#       define LOG_INFO                                             GOOGLE_LOG(INFO)
#       define LOG_WARN                                             GOOGLE_LOG(WARNING)
#       define LOG_ERROR                                            GOOGLE_LOG(ERROR)
#       define LOG_FATAL                                            GOOGLE_LOG(FATAL)
#endif // GOOGLE_LOGGING

#ifndef GOOGLE_LOGGING
#       define EVENT_INFO(...)                                      GOOGLE_LOG_DEBUG_MSG(INFO, __VA_ARGS__)
#       define EVENT_WARN(...)                                      GOOGLE_LOG_DEBUG_MSG(WARNING, __VA_ARGS__)
#       define EVENT_ERROR(...)                                     GOOGLE_LOG_DEBUG_MSG(ERROR, __VA_ARGS__)
#       define EVENT_FATAL(...)                                     GOOGLE_LOG_DEBUG_MSG(FATAL, __VA_ARGS__)
#elif   EVENT_SYSTEM_LOGGING
#       define EVENT_INFO(...)                                      GOOGLE_LOG_DEBUG_MSG(INFO, __VA_ARGS__)
#       define EVENT_WARN(...)                                      GOOGLE_LOG_DEBUG_MSG(WARNING, __VA_ARGS__)
#       define EVENT_ERROR(...)                                     GOOGLE_LOG_DEBUG_MSG(ERROR, __VA_ARGS__)
#       define EVENT_FATAL(...)                                     GOOGLE_LOG_DEBUG_MSG(FATAL, __VA_ARGS__)
#endif // GOOGLE_LOGGING
#endif // __cplusplus
#endif // __EVEMT_LOGGING_H__