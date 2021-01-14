#include <tcp_session.h>
#include <event_loop.h>

#include <tcp_message.h>

namespace evpp
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
}