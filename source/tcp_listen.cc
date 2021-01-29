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
        event_close_flag(0),
        tcp_proble(proble),
        event_share(std::make_shared<EventShare>()),
        event_thread_pool(std::make_shared<EventLoopThreadPool>(loop, event_share))
    {

    }
#else
    TcpListen::TcpListen(EventLoop* loop, const std::shared_ptr<EventLoopThreadPool>& thread_pool, const bool proble) :
        event_base(loop),
        event_close_flag(0),
        tcp_proble(proble),
        event_thread_pool(thread_pool)
    {

    }
#endif
    TcpListen::~TcpListen()
    {
        printf("Delete TcpListen\n");
    }

    bool TcpListen::CreaterListenService(EventSocketPool* socket, TcpServer* server)
    {
        if (nullptr == socket || nullptr == server)
        {
            return false;
        }

        if (ExistsStarts(Status::None))
        {
            return InitialListenService(socket, server, socket->GetSocketPoolSize());
        }
        return false;
    }

    bool TcpListen::DestroyListenService()
    {
        if (event_base->EventThread())
        {
            if (ExistsStoped())
            {
                return true;
            }

            if (ExistsRuning())
            {
                for (u96 i = 0; i < tcp_server.size(); ++i)
                {
                    if (DestroyListenService(event_thread_pool->GetEventLoop(i), tcp_server[i]))
                    {
                        continue;
                    }
                    return false;
                }

                while (tcp_server.size() != event_close_flag.load(std::memory_order_acquire));

                for (auto & var: tcp_server)
                {
                    if (nullptr != var)
                    {
                        delete var;
                        var = nullptr;
                    }
                }

                if (ChangeStatus(Status::Exec, Status::Stop))
                {
                    return event_thread_pool->DestroyEventThreadPool();
                }
                return false;
            }
        }

        return event_base->RunInLoopEx(std::bind((bool(TcpListen::*)(void))&TcpListen::DestroyListenService, this));
    }

    bool TcpListen::DestroyListenService(EventLoop* loop, socket_tcp* server)
    {
        if (nullptr != loop && nullptr != server && loop->EventBasic() == server->loop)
        {
            if (loop->EventThread())
            {
                return DeletedListenService(loop, server);
            }
            return loop->RunInLoopEx(std::bind((bool(TcpListen::*)(EventLoop*, socket_tcp*))&TcpListen::DestroyListenService, this, loop, server));
        }
        return false;
    }

    bool TcpListen::DeletedListenService(EventLoop* loop, socket_tcp* server)
    {
        if (nullptr != loop)
        {
            uv_close(reinterpret_cast<event_handle*>(server), &TcpServer::DefaultCloseListen);
            return true;
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
            if (ChangeStatus(Status::None, Status::Init))
            {
                for (u96 i = 0; i < size; ++i)
                {
                    tcp_server.emplace(tcp_server.begin() + i, new socket_tcp({ server }));
                    {
                        if (ExecuteListenService(event_thread_pool->GetEventLoop(i), tcp_server[i], &socket->GetEventSocket(i)->GetSocketInfo()->addr))
                        {
                            continue;
                        }

                        return false;
                    }
                }
                return ChangeStatus(Status::Init, Status::Exec);
            }
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

    void TcpListen::OnClose()
    {
        if ((1 + event_close_flag.fetch_add(1, std::memory_order_release)) == tcp_server.size())
        {
            printf("¼àÌýÍ£Ö¹\n");
        }
    }
}