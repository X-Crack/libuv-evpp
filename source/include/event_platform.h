#ifndef __EVENT_PLATFORM_H__
#define __EVENT_PLATFORM_H__
#include <assert.h>

#if (defined(_MSC_VER) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32)) && !defined(__SYMBIAN32__)
#   ifndef H_OS_WINDOWS
#       define H_OS_WINDOWS
#       define WIN32_LEAN_AND_MEAN
#       define _CRT_SECURE_NO_WARNINGS
#   endif
#   ifndef H_WINDOWS_API
#       define H_WINDOWS_API
#   endif
#endif

#define USE_PARAMETER(P) (P)

#ifdef H_OS_WINDOWS
#   define FORCEINLINE __forceinline
#   define NOFORCEINLINE __declspec (noinline)
#else
#   define FORCEINLINE __attribute__ ((always_inline))
#   define NOFORCEINLINE __attribute__ ((noinline))
#endif


#ifdef H_OS_WINDOWS
#   define EVPP_THREAD_YIELD SwitchToThread
#else
#   define EVPP_THREAD_YIELD std::this_thread::yield
#endif


#ifdef H_OS_WINDOWS
#pragma warning( disable: 4251 )
#pragma warning( disable: 4996 )
#pragma warning( disable: 4244 4251 4355 4715 4800 4996 4005 4819)
#pragma warning( disable: 4505 )
#pragma warning( disable: 4530 )
#pragma warning( disable: 4577 )
#pragma warning( disable: 4503 )
#pragma warning( disable: 4458 )
#pragma warning( disable: 4100 )
#pragma warning( disable: 4702 )
#endif

#ifdef H_OS_WINDOWS
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "userenv.lib")
#endif
#endif // __EVENT_PLATFORM_H__
