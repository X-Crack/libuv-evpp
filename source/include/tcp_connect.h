#ifndef __TCP_CONNECT_H__
#define __TCP_CONNECT_H__
#include <event_config.h>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class EventSocket;
    class TcpClient;
    class EVPP_EXPORT TcpConnect
    {
    public:
        explicit TcpConnect(EventLoop* loop, socket_tcp* handler, TcpClient* client);
        virtual ~TcpConnect();
    public:
        friend TcpClient;
    public:
        bool ConnectService(const std::unique_ptr<EventSocket>& socket);
        bool DestroyConnect();
    private:
        bool InitTcpService();
        bool CreaterConnect(const sockaddr* addr);
    private:
        EventLoop*                                                      event_base;
        socket_tcp*                                                     tcp_handler;
        std::unique_ptr<socket_connect>                                 tcp_connect;
    };
}
#endif // __TCP_CONNECT_H__
