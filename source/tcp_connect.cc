#include <tcp_connect.h>
#include <tcp_client.h>
#include <event_socket.h>
#include <event_loop.h>
namespace Evpp
{
    TcpConnect::TcpConnect(EventLoop* loop, const std::shared_ptr<socket_tcp>& client) :
        event_loop(loop),
        tcp_client(client),
        tcp_connect(std::make_unique<socket_connect>())

    {

    }

    TcpConnect::~TcpConnect()
    {

    }

    bool TcpConnect::ConnectServers(const std::unique_ptr<EventSocket>& socket, TcpClient* client)
    {
        if (nullptr == tcp_connect->data)
        {
            tcp_connect->data = client;
            {
                if (InitTcpService())
                {
                    if (uv_tcp_nodelay(tcp_client.get(), 1))
                    {
                        printf("³õÊ¼»¯Ê§°Ü\n");
                    }

                    return CreaterConnect(&socket->GetSocketInfo()->addr);
                }
            }
        }
        return false;
    }

    bool TcpConnect::InitTcpService()
    {
        return 0 == uv_tcp_init(event_loop->EventBasic(), tcp_client.get());
    }

    bool TcpConnect::CreaterConnect(const sockaddr* addr)
    {
        return 0 == uv_tcp_connect(tcp_connect.get(), tcp_client.get(), addr, &TcpClient::DefaultConnect);
    }
}