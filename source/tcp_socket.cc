#include <tcp_socket.h>
#include <event_mutex.h>
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
        if (nullptr != handler)
        {
            if (HandlerStatus(handler))
            {
                std::unique_lock<std::recursive_mutex> lock(tcp_mutex);
                if (tcp_info.emplace(index, std::make_shared<SocketInfoEx>()).second)
                {
                    return InitialSockInfo(handler, GetSockInfo(index));
                }
            }
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

    const std::shared_ptr<SocketInfoEx>& TcpSocket::GetSockInfo(const u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(tcp_mutex);
        return std::cref(tcp_info[index]);
    }

    bool TcpSocket::InitialSockInfo(socket_tcp* handler, const std::shared_ptr<SocketInfoEx>& socket)
    {
        if (nullptr != handler)
        {
            return GetSockInfo(handler, socket) && GetPeerInfo(handler, socket);
        }
        return false;
    }

    bool TcpSocket::GetSockInfo(socket_tcp* handler, const std::shared_ptr<SocketInfoEx>& socket)
    {
        if (nullptr == handler)
        {
            return false;
        }
        // 获取与某个套接字关联的本地协议地址
        if (GetSockName(handler, reinterpret_cast<struct sockaddr *>(std::addressof(socket->sockname.addr_storage)), sizeof(struct sockaddr_storage)))
        {
            if (AF_INET == socket->sockname.family)
            {
                if (0 == uv_ip4_name(std::addressof(socket->sockname.addr4), socket->sockname.host_address, std::size(socket->sockname.host_address)))
                {
                    socket->sockname.host = std::move(socket->sockname.host_address);
                    socket->sockname.port = ntohs(socket->sockname.addr4.sin_port);
                    EVENT_INFO("new client enters localhost address: %s localhost port: %u", socket->sockname.host.c_str(), socket->sockname.port);
                }
            }

            if (AF_INET6 == socket->sockname.family)
            {
                if (0 == uv_ip6_name(std::addressof(socket->sockname.addr6), socket->sockname.host_address, std::size(socket->sockname.host_address)))
                {
                    socket->sockname.host = std::move(socket->sockname.host_address);
                    socket->sockname.port = ntohs(socket->sockname.addr6.sin6_port);
                    EVENT_INFO("new client enters localhost address: %s localhost port: %u", socket->sockname.host.c_str(), socket->sockname.port);
                }
            }

            return true;
        }
        return false;
    }

    bool TcpSocket::GetPeerInfo(socket_tcp* handler, const std::shared_ptr<SocketInfoEx>& socket)
    {
        if (nullptr == handler)
        {
            return false;
        }
        // 获取与某个套接字关联的外地协议地址
        if (GetPeerName(handler, reinterpret_cast<struct sockaddr *>(std::addressof(socket->peername.addr_storage)), sizeof(struct sockaddr_storage)))
        {
            if (AF_INET == socket->peername.family)
            {
                if (0 == uv_ip4_name(std::addressof(socket->peername.addr4), socket->peername.host_address, std::size(socket->peername.host_address)))
                {
                    socket->peername.host = std::move(socket->peername.host_address);
                    socket->peername.port = ntohs(socket->peername.addr4.sin_port);
                    EVENT_INFO("new client enters remote address: %s remote port: %u", socket->peername.host.c_str(), socket->peername.port);
                }
            }

            if (AF_INET6 == socket->peername.family)
            {
                if (0 == uv_ip6_name(std::addressof(socket->peername.addr6), socket->peername.host_address, std::size(socket->peername.host_address)))
                {
                    socket->peername.host = std::move(socket->peername.host_address);
                    socket->peername.port = ntohs(socket->peername.addr6.sin6_port);
                    EVENT_INFO("new client enters remote address: %s remote port: %u", socket->peername.host.c_str(), socket->peername.port);
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
            if (HandlerStatus(handler))
            {
                return 0 == uv_tcp_getsockname(handler, addr, &size);
            }
        }
        return false;
    }

    bool TcpSocket::GetPeerName(socket_tcp* handler, struct sockaddr* addr, i32 size)
    {
        if (nullptr != handler)
        {
            if (HandlerStatus(handler))
            {
                return 0 == uv_tcp_getpeername(handler, addr, &size);
            }
        }
        return false;
    }
}
