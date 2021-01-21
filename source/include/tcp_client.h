#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include <config.h>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class EventSocket;
    class TcpConnect;
    class TcpSession;
    class TcpClient
    {
    public:
        explicit TcpClient(EventLoop* loop);
        virtual ~TcpClient();
        friend TcpConnect;
    public:
        bool CreaterClient();
        bool AddListenPort(const std::string& server_address, const u16 port);
    private:
        void DefaultConnect(socket_connect* request);
        void DefaultFailure(socket_connect* request, int status);
        void DefaultDiscons(EventLoop* loop, const u96 index);
        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index);
    private:
        static void DefaultConnect(socket_connect* hanlder, int status);
    private:
        EventLoop*                                          event_loop;
        std::shared_ptr<socket_tcp>                         tcp_client;
        std::unique_ptr<EventSocket>                        tcp_socket;
        std::unique_ptr<TcpConnect>                         tcp_connect;
        std::shared_ptr<TcpSession>                         tcp_session;
    };
}
#endif // __TCP_CLIENT_H__