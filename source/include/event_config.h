﻿#ifndef __EVENT_CONFIG_H__
#define __EVENT_CONFIG_H__
#include <event_platform.h>
#include <uv.h>
#include <event_curl.h>
#include <event_stdint.h>
#include <event_logging.h>
#include <event_untlity.h>

// Use CXX20 coroutine to assist EventLoop operation.
#if defined(_HAS_CXX20) || defined(__cpp_coroutines) && defined(__cpp_impl_coroutine)
#       define EVPP_USE_STL_COROUTINES
#endif

// Use STL-based multi-threaded running mode (std::thread) to close this macro, and use libuv_thread to run multi-threaded
#ifndef EVPP_USE_STL_THREAD
#       define EVPP_USE_STL_THREAD
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



using event_loop = uv_loop_t;
using event_async = uv_async_t;
using event_timer = uv_timer_t;
using event_handle = uv_handle_t;
using event_signal = uv_signal_t;
using event_thread = uv_thread_t;
using event_work = uv_work_t;
using event_poll = uv_poll_t;
using socket_tcp = uv_tcp_t;
using socket_stream = uv_stream_t;
using socket_data = uv_buf_t;
using socket_shutdown = uv_shutdown_t;
using socket_write = uv_write_t;
using socket_connect = uv_connect_t;
using socket_udp = uv_udp_t;
using socket_getaddrinfo = uv_getaddrinfo_t;

// curl 开发文档 https://blog.csdn.net/lijinqi1987/article/details/53996129

#endif // __EVENT_CONFIG_H__
