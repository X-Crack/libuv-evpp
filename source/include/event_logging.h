#ifndef __EVEMT_LOGGING_H__
#define __EVEMT_LOGGING_H__
#include <event_config.h>
// if you need to close the system printout, please use the macro "GOOGLE_LOGGING" to close it
#ifndef GOOGLE_LOGGING

#   ifndef GOOGLE_GLOG_DLL_DECL
#       define GOOGLE_GLOG_DLL_DECL
#   endif

#   ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#       define GLOG_NO_ABBREVIATED_SEVERITIES
#   endif

#       include <logging.h>
#endif // GOOGLE_LOGGING

#if defined(_DEBUG) || defined(DEBUG)
#       define GOOGLE_LOG                         LOG
#else
#       define GOOGLE_LOG
#endif

#ifdef GOOGLE_LOGGING
#       define EVENT_SYSTEM_LOGGING
#endif // GOOGLE_LOGGING

#ifndef GOOGLE_LOGGING
#       define LOG_INFO                        GOOGLE_LOG(INFO)
#       define LOG_WARN                        GOOGLE_LOG(WARNING)
#       define LOG_ERROR                       GOOGLE_LOG(ERROR)
#       define LOG_FATAL                       GOOGLE_LOG(FATAL)
#elif   EVENT_CANCEL_LOGGING
#       define LOG_INFO
#       define LOG_WARN
#       define LOG_ERROR
#       define LOG_FATAL
#endif // GOOGLE_LOGGING

#ifndef GOOGLE_LOGGING
#       define EVENT_INFO                      GOOGLE_LOG(INFO)
#       define EVENT_WARN                      GOOGLE_LOG(WARNING)
#       define EVENT_ERROR                     GOOGLE_LOG(ERROR)
#       define EVENT_FATAL                     GOOGLE_LOG(FATAL)
#elif   EVENT_SYSTEM_LOGGING
#       define EVENT_INFO
#       define EVENT_WARN
#       define EVENT_ERROR
#       define EVENT_FATAL
#endif // GOOGLE_LOGGING

#endif // __EVEMT_LOGGING_H__