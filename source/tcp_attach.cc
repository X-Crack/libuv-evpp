#include <tcp_attach.h>
#include <tcp_client.h>
#include <event_loop.h>
#include <event_timer.h>
namespace Evpp
{
    TcpAttach::TcpAttach(EventLoop* loop, TcpClient* client) : 
        event_base(loop),
        event_timer(std::make_shared<EventTimer>(loop)),
        socket_client(client),
        attach_timer(0),
        attach_delay(0)
    {
        event_timer->SetEventTimerCallback(std::bind(&TcpAttach::OnTimer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    TcpAttach::~TcpAttach()
    {

    }

    void TcpAttach::SetResetConnect(const u64 delay, const u64 timer)
    {
        attach_delay.store(delay);
        attach_timer.store(timer);
    }

    bool TcpAttach::TryRetryConnect()
    {
        return TryRetryConnect(attach_delay, attach_timer);
    }

    bool TcpAttach::TryRetryConnect(const u64 delay, const u64 time)
    {
        return event_timer->AssignTimer(delay, time);
    }

    void TcpAttach::OnTimer(EventLoop* loop, const std::shared_ptr<EventTimer>& timer, const u96 index)
    {
        if (nullptr != loop && 0 > index)
        {
            if (socket_client->ExistsStoped())
            {
                if (socket_client->ConnectService())
                {
                    if (timer->StopedTimer())
                    {
                        return;
                    }
                }
            }
        }
    }
}