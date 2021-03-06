﻿#include <tcp_session.h>
#include <tcp_message.h>
#include <event_loop.h>
#include <event_timer.h>
#include <event_timer_pool.h>

namespace Evpp
{
    TcpSession::TcpSession(EventLoop* loop, socket_tcp* client, const u96 index, const SystemDiscons& discons, const SystemMessage& message, const SystemSendMsg& sendmsg) :
        event_base(loop),
        event_index(index),
        system_discons(discons),
        system_message(message),
        system_sendmsg(sendmsg),
        tcp_socket(client),
        tcp_message(std::make_unique<TcpMessage>(loop, client, std::bind(&TcpSession::OnSystemDiscons, this, std::placeholders::_1), std::bind(&TcpSession::OnSystemMessage, this, std::placeholders::_1), std::bind(&TcpSession::OnSystemSendMsg, this, std::placeholders::_1))),
        event_timer_pool(std::make_unique<EventTimerPool>(loop))
    {
        event_timer_pool->AssignTimer(1, 1000, 1000);
    }

    TcpSession::~TcpSession()
    {

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

    bool TcpSession::RunInLoop(const Handler& function)
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

    bool TcpSession::RunInQueue(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInQueue(function);
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

    void TcpSession::OnSystemDiscons(socket_tcp* socket)
    {
        if (nullptr != system_discons)
        {
            if (RunInQueue(std::bind(system_discons, event_base, socket, event_index)))
            {
                return;
            }
            EVENT_INFO("session left into unexpected error has occurred queue");
        }
    }

    bool TcpSession::OnSystemMessage(const std::shared_ptr<EventBuffer>& Buffer)
    {
        if (nullptr != system_message)
        {
            return system_message(event_base, shared_from_this(), Buffer, event_index);
        }
        return false;
    }

    bool TcpSession::OnSystemSendMsg(const i32 status)
    {
        if (nullptr != system_sendmsg)
        {
            return system_sendmsg(event_base, shared_from_this(), event_index, status);
        }
        return false;
    }
}