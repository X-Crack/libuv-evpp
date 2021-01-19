#include <tcp_session.h>
#include <event_loop.h>
#include <event_timer.h>
#include <event_timer_vesse.h>

#include <tcp_message.h>

namespace Evpp
{
    TcpSession::TcpSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const u96 index, const SystemDiscons& discons, const SystemMessage& message) :
        event_loop(loop),
        tcp_socket(client),
        tcp_message(std::make_unique<TcpMessage>(loop, client, std::bind(&TcpSession::OnSystemDiscons, this), std::bind(&TcpSession::OnSystemMessage, this, std::placeholders::_1))),
        event_timer_vesse(std::make_unique<EventTimerVesse>(loop)),
        self_index(index),
        system_discons(discons),
        system_message(message)
    {

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

    bool TcpSession::RunInLoop(const Functor& function)
    {
        if (nullptr != event_loop)
        {
            return event_loop->RunInLoop(function);
        }
        return false;
    }

    bool TcpSession::AssignTimer(const u96 index, const u64 delay, const u64 repeat)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->AssignTimer(index, delay, repeat);
        }
        return false;
    }

    bool TcpSession::StopedTimer(const u96 index)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->StopedTimer(index);
        }
        return false;
    }

    bool TcpSession::KilledTimer(const u96 index)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->KilledTimer(index);
        }
        return false;
    }

    void TcpSession::ModiyRepeat(const u96 index, const u64 repeat)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->ModiyRepeat(index, repeat);
        }
        return;
    }

    bool TcpSession::ReStarTimer(const u96 index)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->ReStarTimer(index);
        }
        return false;
    }

    bool TcpSession::ReStarTimerEx(const u96 index, const u64 delay, const u64 repeat)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->ReStarTimerEx(index, delay, repeat);
        }
        return false;
    }

    void TcpSession::OnSystemDiscons()
    {
        if (nullptr != system_discons)
        {
            system_discons(event_loop, self_index);
        }
    }

    bool TcpSession::OnSystemMessage(const std::shared_ptr<TcpBuffer>& Buffer)
    {
        if (nullptr != system_message)
        {
            return system_message(event_loop, shared_from_this(), Buffer, self_index);
        }
        return false;
        
    }
}