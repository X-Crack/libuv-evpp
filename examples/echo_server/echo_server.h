#ifndef __ECHO_SERVER_H__
#define __ECHO_SERVER_H__
#include <event_config.h>
#include <memory>
namespace Evpp
{
    class EventShare;
    class EventLoop;
    class EventBuffer;
    class TcpServer;
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
        bool OnMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<EventBuffer>& buffer, const u96 index);
        bool OnSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status);
    private:
        std::shared_ptr<EventShare>                 event_share;
        std::shared_ptr<EventLoop>                  event_base;
        std::unique_ptr<TcpServer>                  tcp_server;
    };
}
#endif // __ECHO_SERVER_H__