#include <tcp_connect.h>
#include <tcp_socket.h>
#include <tcp_client.h>
#include <event_loop.h>
namespace Evpp
{
    TcpConnect::TcpConnect(EventLoop* loop) : 
        event_loop(loop),
        tcp_connect(new socket_connect())
    {
        if (nullptr != tcp_connect)
        {
            if (nullptr == tcp_connect->data)
            {
                tcp_connect->data = this;
            }
        }
    }

    TcpConnect::~TcpConnect()
    {
        if (nullptr != tcp_connect)
        {
            delete tcp_connect;
            tcp_connect = nullptr;
        }
    }

    bool TcpConnect::ConnectServers(const std::unique_ptr<TcpSocket>& socket, TcpClient* client)
    {
        for (u96 i = 0; i < socket->GetListeningPortSize(); ++i) 
        {
            tcp_client.push_back(std::make_unique<socket_tcp>());
            {
                if (nullptr == tcp_client[i]->data)
                {
                    tcp_client[i]->data = client;
                    {
                        if (false == InitTcpService(i))
                        {
                            break;
                        }

                        if (uv_tcp_nodelay(tcp_client[i].get(), 1))
                        {
                            printf("³õÊ¼»¯Ê§°Ü\n");
                        }

                        if (false == ClasTcpService(i, &socket->GetSocketInfo(i)->addr))
                        {
                            break;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool TcpConnect::InitTcpService(const u96 index)
    {
        return 0 == uv_tcp_init(event_loop->EventBasic(), tcp_client[index].get());
    }

    bool TcpConnect::ClasTcpService(const u96 index, const sockaddr* addr)
    {
        return 0 == uv_tcp_connect(tcp_connect, tcp_client[index].get(), addr, &TcpClient::DefaultConnect);
    }
}