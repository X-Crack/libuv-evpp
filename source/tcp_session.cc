#include <tcp_session.h>
#include <tcp_message.h>
#include <event_loop.h>
#include <event_timer.h>
#include <event_timer_pool.h>

namespace Evpp
{
    TcpSession::TcpSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const u96 index, const SystemDiscons& discons, const SystemMessage& message) :
        event_base(loop),
        tcp_socket(client),
        tcp_message(std::make_unique<TcpMessage>(loop, client, std::bind(&TcpSession::OnSystemDiscons, this), std::bind(&TcpSession::OnSystemMessage, this, std::placeholders::_1))),
        event_timer_pool(std::make_unique<EventTimerPool>(loop)),
        self_index(index),
        system_discons(discons),
        system_message(message)
    {

    }

    TcpSession::~TcpSession()
    {
        printf("Delete TcpSession\n");
    }

    bool TcpSession::Send(const char* buf, u96 len, u32 nbufs)
    {
        if (nullptr != tcp_message)
        {
            return tcp_message->Send(buf, len, nbufs);
        }
        return false;
    }

    bool TcpSession::Send(const std::string& buf, u32 nbufs)
    {
        if (nullptr != tcp_message)
        {
            return tcp_message->Send(buf, nbufs);
        }
        return false;
    }

    bool TcpSession::Close()
    {
        if (nullptr != tcp_message)
        {
            return tcp_message->Close();
        }
        return false;
    }

    bool TcpSession::SetSendBlocking(const u32 value)
    {
        return tcp_message->SetSendBlocking(value);
    }

    bool TcpSession::RunInLoop(const Functor& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoop(function);
        }
        return false;
    }

    bool TcpSession::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoopEx(function);
        }
        return false;
    }

    bool TcpSession::AssignTimer(const u96 index, const u64 delay, const u64 repeat)
    {
        if (event_base->EventThread())
        {
            if (nullptr != event_timer_pool)
            {
                return event_timer_pool->AssignTimer(index, delay, repeat);
            }
        }
        return RunInLoop(std::bind(&TcpSession::AssignTimer, this, index, delay, repeat));
    }

    bool TcpSession::StopedTimer(const u96 index)
    {
        if (event_base->EventThread())
        {
            if (nullptr != event_timer_pool)
            {
                return event_timer_pool->StopedTimer(index);
            }
        }
        return RunInLoop(std::bind(&TcpSession::StopedTimer, this, index));
    }

    bool TcpSession::KilledTimer(const u96 index)
    {
        if (event_base->EventThread())
        {
            if (nullptr != event_timer_pool)
            {
                return event_timer_pool->KilledTimer(index);
            }
        }
        return RunInLoop(std::bind(&TcpSession::KilledTimer, this, index));
    }

    void TcpSession::ModiyRepeat(const u96 index, const u64 repeat)
    {
        if (nullptr != event_timer_pool)
        {
            return event_timer_pool->ModiyRepeat(index, repeat);
        }
    }

    bool TcpSession::ReStarTimer(const u96 index)
    {
        if (event_base->EventThread())
        {
            if (nullptr != event_timer_pool)
            {
                return event_timer_pool->ReStarTimer(index);
            }
        }

        return RunInLoop(std::bind(&TcpSession::ReStarTimer, this, index));
    }

    bool TcpSession::ReStarTimerEx(const u96 index, const u64 delay, const u64 repeat)
    {
        if (event_base->EventThread())
        {
            if (nullptr != event_timer_pool)
            {
                return event_timer_pool->ReStarTimerEx(index, delay, repeat);
            }
        }
        return RunInLoop(std::bind(&TcpSession::ReStarTimerEx, this, index, delay, repeat));
    }

    void TcpSession::OnSystemDiscons()
    {
        if (nullptr != system_discons)
        {
            system_discons(event_base, self_index);
        }
    }

    bool TcpSession::OnSystemMessage(const std::shared_ptr<TcpBuffer>& Buffer)
    {
        if (nullptr != system_message)
        {
            return system_message(event_base, std::weak_ptr<TcpSession>(shared_from_this()).lock(), Buffer, self_index);
        }
        return false;
    }
}