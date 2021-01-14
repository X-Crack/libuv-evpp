#ifndef __TCP_SESSION_H__
#define __TCP_SESSION_H__
#include <config.h>
#include <vector>
namespace evpp
{
    class EventLoop;
    class TcpMessage;
    class TcpSession
    {
    public:
        explicit TcpSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client);
        virtual ~TcpSession();
    public:
    private:
        EventLoop*                                                      event_loop;
        std::shared_ptr<socket_tcp>                                     tcp_socket;
        std::unique_ptr<TcpMessage>                                     tcp_message;
    };
}
#endif // __TCP_SESSION_H__