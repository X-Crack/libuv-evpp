#include <tcp_socket.h>
namespace Evpp
{
    TcpSocket::TcpSocket()
    {

    }

    TcpSocket::~TcpSocket()
    {

    }

    bool TcpSocket::AddSockInfo(socket_tcp* handler, const u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(tcp_mutex);
        if (tcp_info.emplace(index, std::make_shared<SocketInfoEx>()).second)
        {
            return InitialSockInfo(handler, GetSockInfo(index));
        }
        return false;
    }

    void TcpSocket::DelSockInfo(const u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(tcp_mutex);
        if (tcp_info.empty())
        {
            return;
        }

        if (tcp_info.find(index) != tcp_info.end(index))
        {
            tcp_info.erase(index);
        }
    }

    SocketInfoEx* TcpSocket::GetSockInfo(const u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(tcp_mutex);
        if (tcp_info.empty())
        {
            return nullptr;
        }

        return tcp_info[index].get();
    }

    bool TcpSocket::InitialSockInfo(socket_tcp* handler, SocketInfoEx* socket)
    {
        return GetSockInfo(handler, socket) && GetPeerInfo(handler, socket);
    }

    bool TcpSocket::GetSockInfo(socket_tcp* handler, SocketInfoEx* socket)
    {
        String address[256];
        // 获取与某个套接字关联的本地协议地址
        if (GetSockName(handler, reinterpret_cast<struct sockaddr *>(&socket->sockname.addr_storage), sizeof(struct sockaddr_storage)))
        {
            if (AF_INET == socket->sockname.family)
            {
                if (0 == uv_ip4_name(&socket->sockname.addr4, address, std::size(address)))
                {
                    socket->sockname.host = address;
                    socket->sockname.port = ntohs(socket->sockname.addr4.sin_port);
                    EVENT_INFO("new client enters localhost address: %s localhost port: %u", address, socket->sockname.port);
                }
            }

            if (AF_INET6 == socket->sockname.family)
            {
                if (0 == uv_ip6_name(&socket->sockname.addr6, address, std::size(address)))
                {
                    socket->sockname.host = address;
                    socket->sockname.port = ntohs(socket->sockname.addr6.sin6_port);
                    EVENT_INFO("new client enters localhost address: %s localhost port: %u", address, socket->sockname.port);
                }
            }

            return true;
        }
        return false;
    }

    bool TcpSocket::GetPeerInfo(socket_tcp* handler, SocketInfoEx* socket)
    {
        String address[256];
        // 获取与某个套接字关联的外地协议地址
        if (GetSockName(handler, reinterpret_cast<struct sockaddr *>(&socket->sockname.addr_storage), sizeof(struct sockaddr_storage)))
        {
            if (AF_INET == socket->peername.family)
            {
                if (0 == uv_ip4_name(&socket->peername.addr4, address, std::size(address)))
                {
                    socket->peername.host = address;
                    socket->peername.port = ntohs(socket->peername.addr4.sin_port);
                    EVENT_INFO("new client enters remote address: %s remote port: %u", address, socket->peername.port);
                }
            }

            if (AF_INET6 == socket->peername.family)
            {
                if (0 == uv_ip6_name(&socket->peername.addr6, address, std::size(address)))
                {
                    socket->peername.host = address;
                    socket->peername.port = ntohs(socket->peername.addr6.sin6_port);
                    EVENT_INFO("new client enters remote address: %s remote port: %u", address, socket->peername.port);
                }
            }

            return true;
        }
        return false;
    }

    bool TcpSocket::GetSockName(socket_tcp* handler, struct sockaddr* addr, i32 size)
    {
        if (nullptr != handler)
        {
            return 0 == uv_tcp_getsockname(handler, addr, &size);
        }
        return false;
    }

    bool TcpSocket::GetPeerName(socket_tcp* handler, struct sockaddr* addr, i32 size)
    {
        if (nullptr != handler)
        {
            return 0 == uv_tcp_getpeername(handler, addr, &size);
        }
        return false;
    }
}
