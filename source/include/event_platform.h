#ifndef __EVENT_PLATFORM_H__
#define __EVENT_PLATFORM_H__
#include <event_arch.h>
#include <event_export.h>
#ifdef H_OS_WINDOWS
#   define FORCEINLINE __forceinline
#   define NOFORCEINLINE __declspec (noinline)
#else
#   define FORCEINLINE __attribute__ ((always_inline))
#   define NOFORCEINLINE __attribute__ ((noinline))
#endif

#ifdef H_OS_WINDOWS
#   if defined(_WIN32)
#       define H_OS_X86
#   elif defined(_WIN64)
#       define H_OS_X64
#   endif
#endif

// Use Boost Assembly
#ifndef EVPP_USE_BOOST_ASSEMBLY

#endif

// Use CXX20 coroutine to assist EventLoop operation.
#if defined(__cpp_coroutines) && !defined(__cpp_impl_coroutine) || defined(_HAS_CXX20)
#   ifndef EVPP_USE_STL_COROUTINES
#       define EVPP_USE_STL_COROUTINES
#   endif
#endif

#ifdef EVPP_USE_BOOST_ASSEMBLY
// b2 install --prefix="f:\common\boost\lib\x86" --toolset=msvc-14.2 --build-type=complete link=static runtime-link=static threading=multi debug release
#   ifndef EVPP_USE_BOOST_THREAD
#       define EVPP_USE_BOOST_THREAD
#   endif

#   ifndef EVPP_USE_BOOST_LOCKFREE_QUEUE
#       define EVPP_USE_BOOST_LOCKFREE_QUEUE
#   endif
#else
// Use STL-based multi-threaded running mode (std::thread) to close this macro, and use libuv_thread to run multi-threaded
#   ifndef EVPP_USE_STL_THREAD
#       define EVPP_USE_STL_THREAD
#   endif

// Use https://github.com/cameron314/concurrentqueue
#   ifndef EVPP_USE_CAMERON314_CONCURRENTQUEUE
#       define EVPP_USE_CAMERON314_CONCURRENTQUEUE
#   endif
#endif

#if (defined(_DEBUG) || defined(DEBUG) || defined(_DEBUG_))
#   define EVENT_DEBUG_MODE
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