cmake_minimum_required(VERSION 3.17)
SET(CMAKE_INSTALL_PREFIX /usr/local/lib)
message("PREFIX = ${CMAKE_INSTALL_PREFIX}")
project(evpp VERSION 0.1.0 LANGUAGES C CXX)
set(evpp_srcs
        source/tcp_attach.cc
        source/tcp_client.cc
        source/tcp_client_service.cc
        source/tcp_connect.cc
        source/tcp_listen.cc
        source/tcp_message.cc
        source/tcp_server.cc
        source/tcp_server_service.cc
        source/tcp_session.cc
        source/tcp_socket.cc
        source/event/event_async.cc
        source/event/event_buffer.cc
        source/event/event_check.cc
        source/event/event_coroutine.cc
        source/event/event_exception.cc
        source/event/event_explicit.cc
        source/event/event_idle.cc
        source/event/event_loop.cc
        source/event/event_loop_thread.cc
        source/event/event_loop_thread_pool.cc
        source/event/event_mutex.cc
        source/event/event_prepare.cc
        source/event/event_queue.cc
        source/event/event_resolve.cc
        source/event/event_share.cc
        source/event/event_signal.cc
        source/event/event_socket.cc
        source/event/event_socket_pool.cc
        source/event/event_status.cc
        source/event/event_timer.cc
        source/event/event_timer_pool.cc
        source/event/event_untlity.cc
        source/event/event_work_queue.cc
        )
set(CMAKE_CXX_STANDARD 20)
set(CXX_FLAGS
        -g
        -fPIC
        -Wall
        -Wno-unused-parameter
        -Wno-unused-function
        -Wunused-variable
        -Wunused-value
        -Wshadow
        -Wcast-align
        -Wwrite-strings
        -Wsign-compare
        -Winvalid-pch
        -fms-extensions
        -Wfloat-equal
        -Wextra
        -std=c++2a)

link_directories(. /usr/local/lib)
include_directories(/usr/local/include)

include_directories(. source/deps/concurrentqueue/source)
include_directories(. source/include)

#target_link_libraries(evpp pthread uv glog)

ADD_LIBRARY(evpp ${evpp_srcs})