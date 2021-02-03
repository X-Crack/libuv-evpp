#ifndef __TCP_ATTACH_H__
#define __TCP_ATTACH_H__
#include <event_config.h>
#include <event_status.h>
#include <atomic>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class EventTimer;
    class TcpClient;
    class TcpAttach : public EventStatus
    {
    public:
        explicit TcpAttach(EventLoop* loop, TcpClient* client);
        virtual ~TcpAttach();
    public:
        void SetResetConnectTimer(const u64 delay, const u64 timer);
    public:
        bool CreaterConnect();
        bool CreaterConnect(const u64 delay, const u64 timer);
        bool DestroyConnect();
    public:
        bool RunInLoop(const Functor& function);
        bool RunInLoopEx(const Handler& function);
    private:
        void OnTimer(EventLoop* loop, const std::shared_ptr<EventTimer>& timer, const u96 index);
    private:
        EventLoop* event_base;
        std::shared_ptr<EventTimer>                         event_timer;
        TcpClient* socket_client;
        std::atomic<u64>                                    attach_delay;
        std::atomic<u64>                                    attach_timer;
    };
}
#endif // __TCP_ATTACH_H__
