#ifndef __EVENT_CONFIG_H__
#define __EVENT_CONFIG_H__
#include <event_platform.h>
#include <event_libuv.h>
#include <event_stdint.h>
#include <event_logging.h>
#include <event_untlity.h>
#include <event_duration.h>
// Use CXX20 coroutine to assist EventLoop operation.
#if defined(__cpp_coroutines) && !defined(__cpp_impl_coroutine) || defined(_HAS_CXX20)
#   ifndef EVPP_USE_STL_COROUTINES
#       define EVPP_USE_STL_COROUTINES
#   endif
#endif

// Use STL-based multi-threaded running mode (std::thread) to close this macro, and use libuv_thread to run multi-threaded
#ifndef EVPP_USE_STL_THREAD
#       define EVPP_USE_STL_THREAD
#endif

// Use https://github.com/cameron314/concurrentqueue
#ifndef EVPP_USE_CAMERON314_CONCURRENTQUEUE
#       define EVPP_USE_CAMERON314_CONCURRENTQUEUE
#endif

#ifndef ___bswap_constant_16
#       define ___bswap_constant_16(x) ((((uint16_t)(x) >> 8) & 0xff) | (((uint16_t)(x) & 0xff) << 8))
#endif

#ifndef ___bswap_constant_32
#       define ___bswap_constant_32(x) (                                                        \
              (((uint32_t)(x) & 0xff000000) >> 24)                                              \
            | (((uint32_t)(x) & 0x00ff0000) >>  8)                                              \
            | (((uint32_t)(x) & 0x0000ff00) <<  8)                                              \
            | (((uint32_t)(x) & 0x000000ff) << 24))
#endif

#ifndef ___bswap_constant_64
#       define ___bswap_constant_64(x) (                                                        \
              (((uint64_t)(x) & 0xff00000000000000ull) >> 56)                                   \
            | (((uint64_t)(x) & 0x00ff000000000000ull) >> 40)                                   \
            | (((uint64_t)(x) & 0x0000ff0000000000ull) >> 24)                                   \
            | (((uint64_t)(x) & 0x000000ff00000000ull) >> 8)                                    \
            | (((uint64_t)(x) & 0x00000000ff000000ull) << 8)                                    \
            | (((uint64_t)(x) & 0x0000000000ff0000ull) << 24)                                   \
            | (((uint64_t)(x) & 0x000000000000ff00ull) << 40)                                   \
            | (((uint64_t)(x) & 0x00000000000000ffull) << 56))
#endif

#ifndef ___bswap_16
#       define ___bswap_16 ___bswap_constant_16
#endif

#ifndef ___bswap_32
#       define ___bswap_32 ___bswap_constant_32
#endif

#ifndef ___bswap_64
#       define ___bswap_64 ___bswap_constant_64
#endif

#define ntohl               ___bswap_32
#define ntohs               ___bswap_16
#define htonl               ___bswap_32
#define htons               ___bswap_16

// curl 开发文档 https://blog.csdn.net/lijinqi1987/article/details/53996129

#endif // __EVENT_CONFIG_H__
