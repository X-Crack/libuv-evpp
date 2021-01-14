#include <tcp_socket.h>
#include <regex>
#include <string>
#include <sstream>
namespace evpp
{
    TcpSocket::TcpSocket()
    {
    }

    TcpSocket::~TcpSocket()
    {

    }

    bool TcpSocket::InitializeTcpSocket()
    {
        for (u96 i = 0; i < socket_info.size(); ++i)
        {
            if (uv_ip4_addr(socket_info[i]->host.c_str(), socket_info[i]->port, &socket_info[i]->addr4))
            {
                if (uv_ip6_addr(socket_info[i]->host.c_str(), socket_info[i]->port, &socket_info[i]->addr6))
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool TcpSocket::AddListenPort(const std::string& server_address, const u16 port)
    {
        for (u96 i = 0; i < socket_info.size(); ++i)
        {
            if (server_address == socket_info[i]->host && port == socket_info[i]->port)
            {
                return false;
            }
        }

        socket_info.push_back(std::make_unique<SocketInfo>(SocketInfo { server_address, port }));
        return true;
    }

    u96 TcpSocket::GetListeningPortSize()
    {
        return socket_info.size();
    }

    std::unique_ptr<SocketInfo>& TcpSocket::GetSocketInfo(const u96 index)
    {
        return socket_info[index];
    }
}