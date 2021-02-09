#include <tcp_attach.h>
#include <tcp_client.h>
#include <event_loop.h>
#include <event_timer.h>
#include <event_status.h>
#include <chrono>
namespace Evpp
{
    TcpAttach::TcpAttach(EventLoop* loop, TcpClient* client) :
        event_base(loop),
        event_timer(std::make_shared<EventTimer>(loop)),
        socket_client(client),
        attach_delay(3000),
        attach_timer(0)
    {
        if (ChangeStatus(Status::None, Status::Init))
        {
            event_timer->SetEventTimerCallback(std::bind(&TcpAttach::OnTimer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        }
    }

    TcpAttach::~TcpAttach()
    {

    }

    void TcpAttach::SetResetConnectTimer(const u64 delay, const u64 timer)
    {
        attach_delay.store(delay);
        attach_timer.store(timer);
    }

    bool TcpAttach::CreaterConnect()
    {
        return CreaterConnect(attach_delay, attach_timer);
    }

    bool TcpAttach::CreaterConnect(const u64 delay, const u64 timer)
    {
        if (ExistsInited() || ExistsStoped() || ExistsExited())
        {
            return event_timer->AssignTimer(delay, timer);
        }
        return false;
    }

    bool TcpAttach::DestroyConnect()
    {
        if (nullptr != event_timer)
        {
            if (ExistsRuning() || ExistsInited())
            {
                return ChangeStatus(Status::Exit) && event_timer->StopedTimer();
            }

            if (ExistsStoped())
            {
                return ChangeStatus(Status::Exit);
            }
        }
        return false;
    }

    bool TcpAttach::RunInLoop(const Functor& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoop(function);
        }
        return false;
    }

    bool TcpAttach::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoopEx(function);
        }
        return false;
    }

    void TcpAttach::OnTimer(EventLoop* loop, const std::shared_ptr<EventTimer>& timer, const u96 index)
    {
        if (nullptr != loop && nullptr != timer && 0 >= index && nullptr != socket_client)
        {
            if (ChangeStatus(Status::Exec))
            {
                if (socket_client->ExistsInited())
                {
                    if (RunInLoopEx(std::bind(&TcpClient::ConnectService, socket_client)))
                    {
                        if (timer->StopedTimer())
                        {
                            if (ChangeStatus(Status::Exec, Status::Stop))
                            {
                                socket_client->tcp_retry.store(1);
                            }
                        }
                    }
                }
            }
        }
    }
}
