#ifndef __EVENT_LIBUV_H__
#define __EVENT_LIBUV_H__
#include <event_platform.h>
#include <uv.h>
#include <atomic>
namespace Evpp
{
    using event_loop = uv_loop_t;
    using event_async = uv_async_t;
    using event_timer = uv_timer_t;
    using event_handle = uv_handle_t;
    using event_signal = uv_signal_t;
    using event_thread = uv_thread_t;
    using event_work = uv_work_t;
    using event_poll = uv_poll_t;
    using event_idle = uv_idle_t;
    using event_prepare = uv_prepare_t;
    using event_check = uv_check_t;
    using socket_tcp = uv_tcp_t;
    using socket_accept = uv_tcp_accept_t;
    using socket_stream = uv_stream_t;
    using socket_data = uv_buf_t;
    using socket_shutdown = uv_shutdown_t;
    using socket_write = uv_write_t;
    using socket_connect = uv_connect_t;
    using socket_udp = uv_udp_t;
    using socket_getaddrinfo = uv_getaddrinfo_t;
}
#endif // __EVENT_LIBUV_H__
