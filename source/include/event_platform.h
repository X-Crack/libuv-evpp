#ifndef __EVENT_PLATFORM_H__
#define __EVENT_PLATFORM_H__
#include <assert.h>

#if defined(_MSC_VER)
#   ifndef H_OS_WINDOWS
#       define H_OS_WINDOWS
#   endif
#   ifndef H_WINDOWS_API
#       define H_WINDOWS_API
#   endif
#endif

#ifdef H_OS_WINDOWS
#pragma warning( disable: 4005 )
#pragma warning( disable: 4251 )
#pragma warning( disable: 4996 )
#pragma warning( disable: 4244 4251 4355 4715 4800 4996 4005 4819)
#pragma warning( disable: 4530 )
#pragma warning( disable: 4577 )
#pragma warning( disable: 4503 )
#pragma warning( disable: 4458 )
#pragma warning( disable: 4100 )
#pragma warning( disable: 4702 )
#endif

#ifdef H_OS_WINDOWS
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "userenv.lib")
#endif
#endif // __EVENT_PLATFORM_H__