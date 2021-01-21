#ifndef __TCP_LISTEN_H__
#define __TCP_LISTEN_H__
#include <config.h>
#include <string>
#include <memory>
namespace Evpp
{
    class EventLoop;
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
        bool InitTcpService(const u96 index);
        bool BindTcpService(const u96 index, const struct sockaddr* addr);
        bool ListenTcpService(const u96 index);
    private:
        EventLoop*                                      event_loop;
        bool                                            tcp_proble;
        std::vector<std::unique_ptr<socket_tcp>>        tcp_server;
    };
}
#endif // __TCP_LISTEN_H__