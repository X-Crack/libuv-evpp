#ifndef __TCP_SESSION_H__
#define __TCP_SESSION_H__
#include <config.h>
#include <string>
#include <vector>
namespace Evpp
{
    class EventLoop;
    class TcpMessage;
    class TcpSession
    {
    public:
        explicit TcpSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client);
        virtual ~TcpSession();
    public:
        bool Send(const char* buf, u96 len, u32 nbufs = 1);
        bool Send(const std::string& buf, u32 nbufs = 1);
    public:
        bool AssignTimer(const u64 delay, const u64 repeat);
    private:
        EventLoop*                                                      event_loop;
        std::shared_ptr<socket_tcp>                                     tcp_socket;
        std::unique_ptr<TcpMessage>                                     tcp_message;
    };
}
#endif // __TCP_SESSION_H__