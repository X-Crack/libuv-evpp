#include <event_socket.h>
namespace Evpp
{
    EventSocket::EventSocket() : socket_t(std::make_unique<SocketInfo>())
    {

    }

    EventSocket::~EventSocket()
    {

    }

    bool EventSocket::CreaterSocket(const std::string& server_address, const u16 port)
    {
        if (nullptr != socket_t)
        {
            if (0 == socket_t->port)
            {
                socket_t->host = server_address;
                socket_t->port = port;
            }
            return InitialSocket();
        }
        return false;
    }

    const std::string& EventSocket::GetHostAddress()
    {
        return socket_t->host;
    }

    const u16 EventSocket::GetHostPort()
    {
        return socket_t->port;
    }

    bool EventSocket::InitialSocket()
    {
        if (nullptr != socket_t)
        {
            if (uv_ip4_addr(socket_t->host.data(), socket_t->port, &socket_t->addr4))
            {
                if (uv_ip6_addr(socket_t->host.data(), socket_t->port, &socket_t->addr6))
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    const std::unique_ptr<SocketInfo>& EventSocket::GetSocketInfo()
    {
        return socket_t;
    }
}