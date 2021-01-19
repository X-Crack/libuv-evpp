#include <tcp_session.h>
#include <event_loop.h>
#include <event_timer.h>

#include <tcp_message.h>

namespace Evpp
{
    TcpSession::TcpSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client) :
        event_loop(loop),
        tcp_socket(client),
        tcp_message(std::make_unique<TcpMessage>(loop, client))
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
}