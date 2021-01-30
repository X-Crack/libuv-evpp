#ifndef __TCP_ATTACH_H__
#define __TCP_ATTACH_H__
#include <config.h>
namespace Evpp
{
    class EventLoop;
    class EventTimer;
    class TcpClient;
    class TcpAttach
    {
    public:
        explicit TcpAttach(EventLoop* loop, TcpClient* client);
        virtual ~TcpAttach();
    public:
        void SetResetConnectTimer(const u64 delay, const u64 timer);
    public:
        bool TryRetryConnect();
        bool TryRetryConnect(const u64 delay, const u64 timer);
    private:
        void OnTimer(EventLoop* loop, const std::shared_ptr<EventTimer>& timer, const u96 index);
    private:
        EventLoop*                                          event_base;
        std::shared_ptr<EventTimer>                         event_timer;
        TcpClient*                                          socket_client;
        std::atomic<u64>                                    attach_timer;
        std::atomic<u64>                                    attach_delay;
    };
}
#endif // __TCP_ATTACH_H__