#include <tcp_listen.h>
#include <tcp_server.h>
#include <event_share.h>
#include <event_loop.h>
#include <event_loop_thread_pool.h>
#include <event_socket.h>
#include <event_socket_pool.h>
namespace Evpp
{
#ifdef H_OS_WINDOWS
    TcpListen::TcpListen(EventLoop* loop, const bool proble) :
        event_base(loop),
        tcp_proble(proble),
        event_share(std::make_shared<EventShare>()),
        event_thread_pool(std::make_shared<EventLoopThreadPool>(loop, event_share))
    {

    }
#else
    TcpListen::TcpListen(EventLoop* loop, const std::shared_ptr<EventLoopThreadPool>& thread_pool, const bool proble) :
        event_base(loop),
        tcp_proble(proble),
        event_thread_pool(thread_pool)
    {

    }
#endif
    TcpListen::~TcpListen()
    {

    }

    bool TcpListen::CreaterListenService(EventSocketPool* socket, TcpServer* server)
    {
        if (nullptr == socket || nullptr == server)
        {
            return false;
        }
        return InitialListenService(socket, server, socket->GetSocketPoolSize());
    }

    bool TcpListen::DestroyListenService()
    {
        for (u96 i = 0; i < tcp_server.size(); ++i)
        {
            if (DestroyListenService(event_thread_pool->GetEventLoop(i), reinterpret_cast<event_handle*>(tcp_server[i].get())))
            {
                continue;
            }
            return false;
        }

        return event_thread_pool->DestroyEventThreadPool();
    }

    bool TcpListen::DestroyListenService(EventLoop* loop, event_handle* server)
    {
        if (nullptr != loop)
        {
            if (loop->EventThread())
            {
                uv_close(server, 0);
                return true;
            }
            return loop->RunInLoopEx(std::bind((bool(TcpListen::*)(EventLoop*, event_handle*))&TcpListen::DestroyListenService, this, loop, server));
        }
        return false;
    }

    bool TcpListen::InitialListenService(EventSocketPool* socket, TcpServer* server, const u96 size)
    {
        if (nullptr != socket && nullptr != server)
        {
#ifdef H_OS_WINDOWS
            if (!InitEventThreadPools(size))
            {
                return false;
            }
#endif
            for (u96 i = 0; i < size; ++i)
            {
                tcp_server.emplace(tcp_server.begin() + i, std::make_shared<socket_tcp>(socket_tcp{ server }));
                {
                    if (ExecuteListenService(event_thread_pool->GetEventLoop(i), tcp_server[i].get(), &socket->GetEventSocket(i)->GetSocketInfo()->addr))
                    {
                        continue;
                    }

                    return false;
                }
            }
            return true;
        }
        return false;
    }

    bool TcpListen::InitEventThreadPools(const u96 size)
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


    bool TcpListen::ExecuteListenService(EventLoop* loop, socket_tcp* server, const sockaddr* addr)
    {
        if (loop->EventThread())
        {
            if (InitTcpService(loop, server))
            {
                if (0 == uv_tcp_simultaneous_accepts(server, 0))
                {
                    if (tcp_proble)
                    {
                        if (uv_tcp_nodelay(server, 1))
                        {
                            printf("³õÊ¼»¯Ê§°Ü\n");
                        }
                    }

                    if (false == BindTcpService(server, addr))
                    {
                        return false;
                    }

                    if (false == ListenTcpService(server))
                    {
                        return false;
                    }
                    return true;
                }
            }
            return false;
        }
        return loop->RunInLoopEx(std::bind(&TcpListen::ExecuteListenService, this, loop, server, addr));
    }

    bool TcpListen::InitTcpService(EventLoop* loop, socket_tcp* server)
    {
        return 0 == uv_tcp_init(loop->EventBasic(), server);
    }

    bool TcpListen::BindTcpService(socket_tcp* server, const sockaddr* addr)
    {
        return 0 == uv_tcp_bind(server, addr, 0);
    }

    bool TcpListen::ListenTcpService(socket_tcp* server)
    {
        return 0 == uv_listen(reinterpret_cast<socket_stream*>(server), SOMAXCONN, &TcpServer::OnDefaultAccepts);
    }
}