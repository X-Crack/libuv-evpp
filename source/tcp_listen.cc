#include <tcp_listen.h>
#include <tcp_server.h>
#include <event_share.h>
#include <event_loop.h>
#include <event_loop_thread_pool.h>
#include <event_socket.h>
#include <event_socket_pool.h>
namespace Evpp
{
    TcpListen::TcpListen(EventLoop* loop, const bool proble) :
        event_loop(loop),
        tcp_proble(proble),
        event_share(std::make_shared<EventShare>()),
        event_thread_pool(std::make_unique<EventLoopThreadPool>(loop, event_share))
    {

    }

    TcpListen::~TcpListen()
    {

    }

    bool TcpListen::CreaterListenService(const std::unique_ptr<EventSocketPool>& socket, TcpServer* server)
    {
        if (nullptr == socket || nullptr == server)
        {
            return false;
        }
        return CreaterListenService(socket, socket->GetSocketPoolSize(), server);
    }

    bool TcpListen::CreaterListenService(const std::unique_ptr<EventSocketPool>& socket, const u96 size, TcpServer* server)
    {
        if (InitialListenService(size))
        {
            for (u96 i = 0; i < size; ++i)
            {
                tcp_server.push_back(std::make_unique<socket_tcp>());
                {
                    if (nullptr == tcp_server[i]->data)
                    {
                        tcp_server[i]->data = server;
                    }

                    if (InitTcpService(i))
                    {
                        if (0 == uv_tcp_simultaneous_accepts(tcp_server[i].get(), 0))
                        {
                            if (tcp_proble)
                            {
                                if (uv_tcp_nodelay(tcp_server[i].get(), 1))
                                {
                                    printf("³õÊ¼»¯Ê§°Ü\n");
                                }
                            }

                            if (false == BindTcpService(i, &socket->GetEventSocket(i)->GetSocketInfo()->addr))
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
            return true;
        }
        return false;
    }

    bool TcpListen::InitialListenService(const u96 size)
    {
        if (nullptr != event_share && nullptr != event_thread_pool)
        {
            if (event_share->CreaterLoops(size))
            {
                return event_thread_pool->CreaterEventThreadPool(size);
            }
        }
        return false;
    }

    bool TcpListen::InitTcpService(const u96 index)
    {
        return 0 == uv_tcp_init(event_thread_pool->GetEventLoop(index)->EventBasic(), tcp_server[index].get());
    }

    bool TcpListen::BindTcpService(const u96 index, const struct sockaddr* addr)
    {
        return 0 == uv_tcp_bind(tcp_server[index].get(), addr, 0);
    }

    bool TcpListen::ListenTcpService(const u96 index)
    {
        return 0 == uv_listen(reinterpret_cast<socket_stream*>(tcp_server[index].get()), SOMAXCONN, &TcpServer::OnDefaultAccepts);
    }
}