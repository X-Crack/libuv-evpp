#ifndef __EVEMT_LOGGING_H__
#define __EVEMT_LOGGING_H__
#include <event_config.h>
#ifndef GOOGLE_GLOG_DLL_DECL
#define GOOGLE_GLOG_DLL_DECL
#endif

#ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif

#include <logging.h>

# define LOG_INFO						LOG(INFO)
# define LOG_WARN						LOG(WARNING)
# define LOG_ERROR						LOG(ERROR)
# define LOG_FATAL						LOG(FATAL)

#endif // __EVEMT_LOGGING_H__