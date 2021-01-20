#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include <config.h>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class TcpSocket;
    class TcpConnect;
    class TcpClient
    {
    public:
        explicit TcpClient(EventLoop* loop);
        virtual ~TcpClient();
        friend TcpConnect;
    public:
        bool CreaterClient();
        TcpClient& AddListenPort(const std::string& server_address, const u16 port);
    private:
        void DefaultConnect(socket_connect* request);
        void DefaultRefuser(socket_connect* request, int status);
    private:
        static void DefaultConnect(socket_connect* hanlder, int status);
    private:
        EventLoop*                                          event_loop;
        std::unique_ptr<TcpSocket>                          tcp_socket;
        std::unique_ptr<TcpConnect>                         tcp_connect;
    };
}
#endif // __TCP_CLIENT_H__