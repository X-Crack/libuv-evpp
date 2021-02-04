#ifndef __EVENT_CURL_H__
#define __EVENT_CURL_H__
#include <event_platform.h>
#ifndef EVENT_USE_CURL

#       ifndef CURL_STATICLIB
#           define CURL_STATICLIB 1
#       endif

#       ifndef CURL_NO_OLDIES
#           define CURL_NO_OLDIES 1
#       endif

#       ifndef BUILDING_LIBCURL
#           define BUILDING_LIBCURL 1
#       endif

#       ifndef CURL_HIDDEN_SYMBOLS
#           define CURL_HIDDEN_SYMBOLS 1
#       endif

#       ifndef CURL_MAX_READ_SIZE
#           define CURL_MAX_READ_SIZE 65536 * 32
#       endif

#       ifndef CURL_MAX_WRITE_SIZE
#           define CURL_MAX_WRITE_SIZE 65536 * 16
#       endif

#       include <event_curl_config.h>
#       include <curl/curl.h>
#       include <event_curl_untlity.h>

#ifdef H_OS_WINDOWS
#       pragma comment(lib, "crypt32.lib")
#endif
#else  // H_OS_WINDOWS
#endif // EVENT_USE_CURL

// SSL 问题 https://www.cnblogs.com/cherishui/p/13997087.html
#endif // __EVENT_CURL_H__
