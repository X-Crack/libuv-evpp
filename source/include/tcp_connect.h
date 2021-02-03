#ifndef __TCP_CONNECT_H__
#define __TCP_CONNECT_H__
#include <event_config.h>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class EventSocket;
    class TcpClient;
    class TcpConnect
    {
    public:
        explicit TcpConnect(EventLoop* loop, const std::shared_ptr<socket_tcp>& handler, TcpClient* client);
        virtual ~TcpConnect();
    public:
        bool ConnectService(const std::unique_ptr<EventSocket>& socket);
    private:
        bool InitTcpService();
        bool CreaterConnect(const sockaddr* addr);
    private:
        EventLoop* event_base;
        std::shared_ptr<socket_tcp>                             tcp_handler;
        std::unique_ptr<socket_connect>                         tcp_connect;
    };
}
#endif // __TCP_CONNECT_H__
