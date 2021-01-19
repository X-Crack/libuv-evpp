#ifndef __TCP_SESSION_H__
#define __TCP_SESSION_H__
#include <config.h>
#include <string>
#include <vector>
namespace Evpp
{
    class EventLoop;
    class EventTimerVesse;
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
        bool RunInLoop(const Functor& function);
    public:
        bool AssignTimer(const u96 index, const u64 delay, const u64 repeat);
        bool StopedTimer(const u96 index);
        bool KilledTimer(const u96 index);
        void ModiyRepeat(const u96 index, const u64 repeat);
        bool ReStarTimer(const u96 index);
        bool ReStarTimerEx(const u96 index, const u64 delay, const u64 repeat);
    private:
        EventLoop*                                                      event_loop;
        std::shared_ptr<socket_tcp>                                     tcp_socket;
        std::unique_ptr<TcpMessage>                                     tcp_message;
        std::unique_ptr<EventTimerVesse>                                event_timer_vesse;
    };
}
#endif // __TCP_SESSION_H__