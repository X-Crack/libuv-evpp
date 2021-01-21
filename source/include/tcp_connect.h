#ifndef __TCP_CONNECT_H__
#define __TCP_CONNECT_H__
#include <config.h>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class EventSocket;
    class TcpClient;
    class TcpConnect
    {
    public:
        explicit TcpConnect(EventLoop* loop, const std::shared_ptr<socket_tcp>& client);
        virtual ~TcpConnect();
    public:
        bool ConnectServers(const std::unique_ptr<EventSocket>& socket, TcpClient* client);
    private:
        bool InitTcpService();
        bool CreaterConnect(const sockaddr* addr);
    private:

    private:
        EventLoop*                                              event_loop;
        std::shared_ptr<socket_tcp>                             tcp_client;
        std::unique_ptr<socket_connect>                         tcp_connect;
    };
}
#endif // __TCP_CONNECT_H__