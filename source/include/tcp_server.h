﻿#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
#include <event_config.h>
#include <event_status.h>
#include <atomic>
#include <memory>
#include <queue>
#include <mutex>
namespace Evpp
{
    class EventShare;
    class EventLoop;
    class EventLoopThreadPool;
    class EventSocketPool;
    class EventMutex;
    class TcpListen;
    class TcpSession;
    class TcpSocket;
    class TcpServer : public EventStatus
    {
    public:
        explicit TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share);
        explicit TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share, const InterfaceAccepts& accepts, const InterfaceDiscons& discons, const InterfaceMessage& message, const InterfaceSendMsg& sendmsg);
        virtual ~TcpServer();
    public:
        friend TcpListen;
    public:
        bool CreaterServer(const u96 thread_size);
        // you can not use the main thread to clean up work, start a new thread to clean up
        // to ensure that the cleanup can be completed smoothly, please use blocking mode as much as possible.
        bool DestroyServer();
        bool AddListenPort(const std::string& server_address, const u16 port);
        bool Send(const u96 index, const char* buf, u96 len, u32 nbufs = 1);
        bool Send(const u96 index, const std::string& buf, u32 nbufs = 1);
        bool Close(const u96 index);
        void SetKeepaLive(const u32 time);
        u32  GetHardwareThreads();
    public:
        void SetAcceptsCallback(const InterfaceAccepts& accepts);
        void SetDisconsCallback(const InterfaceDiscons& discons);
        void SetMessageCallback(const InterfaceMessage& message);
        void SetSendMsgCallback(const InterfaceSendMsg& sendmsg);
    public:
        bool RunInLoop(const Handler& function);
        bool RunInLoopEx(const Handler& function);
        bool RunInQueue(const Handler& function);
    private:
        bool CreaterSession(EventLoop* loop, socket_tcp* client, const u96 index);
        bool InitialSession(EventLoop* loop, socket_tcp* client, const u96 index);
        bool InitialAccepts(EventLoop* loop, socket_stream* server, socket_tcp* client, const u96 index);
        void DeletedSession(const u96 index);
        bool CleanedSession();
        const std::shared_ptr<TcpSession>& GetSession(const u96 index);
    private:
        bool DefaultAccepts(EventLoop* loop, socket_stream* server, socket_tcp* client, const u96 index);
        bool DefaultAccepts(socket_stream* server, i32 status);
    private:
        void DefaultDiscons(EventLoop* loop, const u96 index);
        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<EventBuffer>& buffer, const u96 index);
        bool DefaultSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status);
    private:
        bool SocketClose(socket_tcp* handler);
        bool SocketShutdown(socket_tcp* handler);
        const u96 GetPlexingIndex(u96 index = 0);
        const u96 GetClientIndex();
    private:
        static void OnDefaultAccepts(socket_stream* handler, int status);
        static void OnDefaultClose(event_handle* handler);
        static void OnDefaultShutdown(socket_shutdown* shutdown, int status);
        static void OnDefaultListen(event_handle* handler);
    private:
        bool DestroyService();
    private:
        EventLoop*                                                              event_base;
        std::shared_ptr<EventShare>                                             event_share;
        std::shared_ptr<EventLoopThreadPool>                                    event_thread_pool;
        std::unique_ptr<EventSocketPool>                                        event_socket;
        std::unique_ptr<EventMutex>                                             event_mutex_clean;
        InterfaceAccepts                                                        socket_accepts;
        InterfaceDiscons                                                        socket_discons;
        InterfaceMessage                                                        socket_message;
        InterfaceSendMsg                                                        socket_sendmsg;
        std::unique_ptr<TcpSocket>                                              tcp_socket;
        std::unique_ptr<TcpListen>                                              tcp_listen;
        std::atomic<u96>                                                        tcp_index;
        std::atomic<u32>                                                        tcp_keepalive;
        std::unordered_map<u96, std::shared_ptr<TcpSession>>                    tcp_session;
        std::priority_queue<u96, std::deque<u96>, std::greater<u96>>            tcp_index_multiplexing;
        std::recursive_mutex                                                    tcp_recursive_mutex;
    };
}
#endif // __TCP_SERVER_H__
