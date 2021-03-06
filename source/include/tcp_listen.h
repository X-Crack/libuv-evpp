﻿#ifndef __TCP_LISTEN_H__
#define __TCP_LISTEN_H__
#include <event_config.h>
#include <event_status.h>
#include <event_mutex.h>
#include <string>
#include <memory>
namespace Evpp
{
    class EventShare;
    class EventLoop;
    class EventLoopThreadPool;
    class EventSocketPool;
    class EventSemaphore;
    class TcpServer;
    class EVPP_EXPORT TcpListen final : public EventStatus
    {
    public:
#ifdef H_OS_WINDOWS
        explicit TcpListen(EventLoop* loop, const bool proble = true);
#else
        explicit TcpListen(EventLoop* loop, const std::shared_ptr<EventLoopThreadPool>& thread_pool, const bool proble = true);
#endif
        virtual ~TcpListen();
    public:
        friend TcpServer;
    public:
        bool CreaterListenService(EventSocketPool* socket, TcpServer* server);
        bool DestroyListenService();
    private:
        bool DestroyListenService(EventLoop* loop, socket_tcp* server, const u96 index);
        bool DeletedListenService();
    private:
        bool InitialListenService(EventSocketPool* socket, TcpServer* server, const u96 size);
#ifdef H_OS_WINDOWS
        bool InitEventThreadPools(const u96 size);
#endif
        bool ExecuteListenService(EventLoop* loop, socket_tcp* server, const sockaddr* addr, const u96 index);
    private:
        bool InitTcpService(EventLoop* loop, socket_tcp* server);
        bool BindTcpService(socket_tcp* server, const sockaddr* addr);
        bool ListenTcpService(socket_tcp* server);
        bool ListenTcpServiceImpl(const i32 status);
    private:
        void OnClose();
    private:
        EventLoop*                                                      event_base;
#ifdef H_OS_WINDOWS
        std::shared_ptr<EventShare>                                     event_share;
#endif
        std::shared_ptr<EventLoopThreadPool>                            event_thread_pool;
        EventLocking                                                    event_locking;
        bool                                                            tcp_proble;
        std::vector<std::unique_ptr<socket_tcp>>                        tcp_server;
    };
}
#endif // __TCP_LISTEN_H__
