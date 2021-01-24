#ifndef __ECHO_SERVER_H__
#define __ECHO_SERVER_H__
#include <config.h>
#include <memory>
namespace Evpp
{
    class EventShare;
    class EventLoop;
    class TcpServer;
    class TcpBuffer;
    class TcpSession;

    class EchoServer
    {
    public:
        explicit EchoServer();
        virtual ~EchoServer();
    public:
        void RunExamples();
    private:
        bool OnAccepts(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index);
        bool OnDiscons(EventLoop* loop, const u96 index);
        bool OnMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index);
    private:
        std::shared_ptr<EventShare>                 event_share;
        std::shared_ptr<EventLoop>                  event_loop;
        std::unique_ptr<TcpServer>                  tcp_server;
    };
}
#endif // __ECHO_SERVER_H__