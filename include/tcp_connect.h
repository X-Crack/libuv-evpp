#ifndef __TCP_CONNECT_H__
#define __TCP_CONNECT_H__
#include <config.h>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class EventSocket;
    class TcpConnect
    {
    public:
        explicit TcpConnect(EventLoop* loop, const std::shared_ptr<socket_tcp>& client);
        virtual ~TcpConnect();
    public:
        bool ConnectServers(const std::unique_ptr<EventSocket>& socket, void* client);
    private:
        bool InitTcpService();
        bool CreaterConnect(const sockaddr* addr);
    private:
        bool ConnectService(const std::unique_ptr<EventSocket>& socket);
        bool InitialConnect(void* client);
    private:
        EventLoop*                                              event_base;
        std::shared_ptr<socket_tcp>                             tcp_client;
        std::unique_ptr<socket_connect>                         tcp_connect;
    };
}
#endif // __TCP_CONNECT_H__