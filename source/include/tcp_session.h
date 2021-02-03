#ifndef __TCP_SESSION_H__
#define __TCP_SESSION_H__
#include <event_config.h>
#include <string>
#include <vector>
namespace Evpp
{
    class EventLoop;
    class EventTimerPool;
    class TcpMessage;
    class TcpSession : public std::enable_shared_from_this<TcpSession>
    {
    public:
        typedef std::function<void(EventLoop*, const u96)>                                                                          SystemDiscons;
        typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const std::shared_ptr<EventBuffer>&, const u96)> SystemMessage;
        typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const u96, const i32)>                           SystemSendMsg;
    public:
        explicit TcpSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const u96 index, const SystemDiscons& discons, const SystemMessage& message, const SystemSendMsg& sendmsg);
        virtual ~TcpSession();
    public:
        bool Send(const char* buf, u96 len, u32 nbufs = 1);
        bool Send(const std::string& buf, u32 nbufs = 1);
    public:
        bool Close();
    public:
        bool SetSendBlocking(const u32 value = 0);
    public:
        bool RunInLoop(const Functor& function);
        bool RunInLoopEx(const Handler& function);
    public:
        bool AssignTimer(const u96 index, const u64 delay, const u64 repeat);
        bool StopedTimer(const u96 index);
        void KilledTimer(const u96 index);
        void ModiyRepeat(const u96 index, const u64 repeat);
        bool ReStarTimer(const u96 index);
        bool ReStarTimerEx(const u96 index, const u64 delay, const u64 repeat);
        u96  GetSelfIndex() { return self_index; };
    private:
        void OnSystemDiscons();
        bool OnSystemMessage(const std::shared_ptr<EventBuffer>& Buffer);
        bool OnSystemSendMsg(const i32 status);
    private:
        EventLoop* event_base;
        u96                                                             self_index;
        SystemDiscons                                                   system_discons;
        SystemMessage                                                   system_message;
        SystemSendMsg                                                   system_sendmsg;
        std::shared_ptr<socket_tcp>                                     tcp_socket;
        std::unique_ptr<TcpMessage>                                     tcp_message;
        std::unique_ptr<EventTimerPool>                                 event_timer_pool;
    };
}
#endif // __TCP_SESSION_H__
