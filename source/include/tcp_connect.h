#ifndef __TCP_CONNECT_H__
#define __TCP_CONNECT_H__
#include <config.h>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class TcpSocket;
    class TcpClient;
    class TcpConnect
    {
    public:
        explicit TcpConnect(EventLoop* loop);
        virtual ~TcpConnect();
    public:
        bool ConnectServers(const std::unique_ptr<TcpSocket>& socket, TcpClient* client);
    private:
        bool InitTcpService(const u96 index);
        bool ClasTcpService(const u96 index, const sockaddr* addr);
    private:

    private:
        EventLoop*                                              event_loop;
        socket_connect*                                         tcp_connect;
        std::vector<std::unique_ptr<socket_tcp>>                tcp_client;
    };
}
#endif // __TCP_CONNECT_H__