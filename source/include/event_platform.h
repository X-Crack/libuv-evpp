#ifndef __EVENT_PLATFORM_H__
#define __EVENT_PLATFORM_H__
#include <event_arch.h>

#ifdef H_OS_WINDOWS
#   define FORCEINLINE __forceinline
#   define NOFORCEINLINE __declspec (noinline)
#else
#   define FORCEINLINE __attribute__ ((always_inline))
#   define NOFORCEINLINE __attribute__ ((noinline))
#endif

#ifdef H_OS_WINDOWS
#   if !defined(_DLL) && !defined(_DEBUG)
#       pragma message("Compilation mode MT")
#   elif !defined(_DLL) && defined(_DEBUG)
#       pragma message("Compilation mode MTd")
#   elif defined(_DLL) && !defined(_DEBUG)
#       pragma message("Compilation mode MD")
#   elif defined(_DLL) && defined(_DEBUG)
#       pragma message("Compilation mode MDd")
#   endif
#endif

#ifdef H_OS_WINDOWS
#   ifdef _WIN32
#       define H_OS_X86
#   elif _WIN64
#       define H_OS_X64
#   endif
#endif

#ifdef H_OS_WINDOWS
#   define EVPP_THREAD_YIELD SwitchToThread
#else
#   define EVPP_THREAD_YIELD std::this_thread::yield
#endif

#define USE_PARAMETER(P) (P)

#ifdef H_OS_WINDOWS
#   pragma warning( disable: 4251 )
#   pragma warning( disable: 4996 )
#   pragma warning( disable: 4244 4251 4355 4715 4800 4996 4005 4819)
#   pragma warning( disable: 4505 )
#   pragma warning( disable: 4530 )
#   pragma warning( disable: 4577 )
#   pragma warning( disable: 4503 )
#   pragma warning( disable: 4458 )
#   pragma warning( disable: 4100 )
#   pragma warning( disable: 4702 )
#endif

#ifdef H_OS_WINDOWS
#   pragma comment(lib, "ws2_32.lib")
#   pragma comment(lib, "wldap32.lib")
#   pragma comment(lib, "iphlpapi.lib")
#   pragma comment(lib, "psapi.lib")
#   pragma comment(lib, "userenv.lib")
#endif

#endif // __EVENT_PLATFORM_H__
