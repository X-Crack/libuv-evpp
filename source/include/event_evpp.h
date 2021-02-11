#ifndef __EVENT_EVPP_H__
#define __EVENT_EVPP_H__
#       include <event_platform.h>
#ifdef H_OS_WINDOWS_MSVC
#   if __cplusplus == 199711L
#       error "Settings C/C++ -> Command Line -> Other Commands -> Add: /Zc:__cplusplus";
#   elif __cplusplus > 201703L
#       include <event_config.h>
#       include <event_status.h>
#       include <event_share.h>
#       include <event_coroutine.h>
#       include <event_loop.h>
#       include <event_loop_thread.h>
#       include <event_loop_thread_pool.h>
#       include <event_async.h>
#       include <event_queue.h>
#       include <event_buffer.h>
#       include <event_timer.h>
#       include <event_timer_pool.h>
#       include <event_signal.h>
#       include <event_socket.h>
#       include <event_socket_pool.h>
#       include <event_resolve.h>
#       include <event_work_queue.h>
#       include <event_idle.h>
#       include <event_prepare.h>
#       include <event_check.h>
#       include <event_explicit.h>

#       include <tcp_connect.h>
#       include <tcp_client.h>
#       include <tcp_client_service.h>
#       include <tcp_attach.h>
#       include <tcp_listen.h>
#       include <tcp_message.h>
#       include <tcp_session.h>
#       include <tcp_server.h>
#       include <tcp_server_service.h>
#       include <tcp_socket.h>
#   else
#       error "Please enable C/C++ 2a support";
#   endif
#endif

#endif // __EVENT_EVPP_H__
