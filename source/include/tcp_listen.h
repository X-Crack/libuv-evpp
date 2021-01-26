#ifndef __TCP_LISTEN_H__
#define __TCP_LISTEN_H__
#include <config.h>
#include <string>
#include <memory>
namespace Evpp
{
    class EventShare;
    class EventLoop;
    class EventLoopThreadPool;
    class EventSocketPool;
    class TcpServer;
    class TcpListen
    {
    public:
        explicit TcpListen(EventLoop* loop, const bool proble = true);
        virtual ~TcpListen();
    public:
        bool CreaterListenService(const std::unique_ptr<EventSocketPool>& socket, TcpServer* server);
    private:
        bool CreaterListenService(const std::unique_ptr<EventSocketPool>& socket, const u96 size, TcpServer* server);
        bool InitialListenService(const u96 size);
        bool InitTcpService(EventLoop* loop,const u96 index);
        bool BindTcpService(const u96 index, const struct sockaddr* addr);
        bool ListenTcpService(const u96 index);
    private:
        EventLoop*                                      event_base;
        std::shared_ptr<EventShare>                     event_share;
        std::unique_ptr<EventLoopThreadPool>            event_thread_pool;
        bool                                            tcp_proble;
        std::vector<std::unique_ptr<socket_tcp>>        tcp_server;
    };
}
#endif // __TCP_LISTEN_H__