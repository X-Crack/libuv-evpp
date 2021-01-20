#include <tcp_listen.h>
#include <event_loop.h>
#include <tcp_server.h>
#include <tcp_socket.h>
namespace Evpp
{
    TcpListen::TcpListen(EventLoop* loop, const bool proble) :
        event_loop(loop),
        tcp_proble(proble)
    {

    }

    TcpListen::~TcpListen()
    {

    }

    bool TcpListen::CreaterListenService(const std::unique_ptr<TcpSocket>& socket, TcpServer* server)
    {
        for (u96 i = 0; i < socket->GetListeningPortSize(); ++i)
        {
            tcp_server.push_back(std::make_unique<socket_tcp>());
            {
                if (nullptr == tcp_server[i]->data)
                {
                    tcp_server[i]->data = server;
                    {
                        if (InitTcpService(i))
                        {
                            uv_tcp_simultaneous_accepts(tcp_server[i].get(), 0);

                            if (tcp_proble)
                            {
                                if (uv_tcp_nodelay(tcp_server[i].get(), 1))
                                {
                                    printf("³õÊ¼»¯Ê§°Ü\n");
                                }
                            }

                            if (false == BindTcpService(i, &socket->GetSocketInfo(i)->addr))
                            {
                                return false;
                            }

                            if (false == ListenTcpService(i))
                            {
                                return false;
                            }
                        }
                    }
                }
            }
        }
        return true;
    }

    bool TcpListen::InitTcpService(const u96 index)
    {
        return 0 == uv_tcp_init(event_loop->EventBasic(), tcp_server[index].get());
    }

    bool TcpListen::BindTcpService(const u96 index, const struct sockaddr* addr)
    {
        return 0 == uv_tcp_bind(tcp_server[index].get(), addr, 0);
    }

    bool TcpListen::ListenTcpService(const u96 index)
    {
        return 0 == uv_listen(reinterpret_cast<socket_stream*>(tcp_server[index].get()), SOMAXCONN, &TcpServer::DefaultAccepts);
    }
}