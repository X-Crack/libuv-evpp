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
        event_close_flag(1),
        event_close_flag_ex(1),
        event_start_flag(1),
        tcp_proble(proble),
        event_share(std::make_shared<EventShare>()),
        event_thread_pool(std::make_shared<EventLoopThreadPool>(loop, event_share))
    {

    }
#else
    TcpListen::TcpListen(EventLoop* loop, const std::shared_ptr<EventLoopThreadPool>& thread_pool, const bool proble) :
        event_base(loop),
        event_close_flag(0),
        event_close_flag_ex(0),
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

        if (ExistsStarts(Status::None))
        {
            if (InitialListenService(socket, server, socket->GetSocketPoolSize()))
            {
                return true;
            }
        }
        return false;
    }

    bool TcpListen::DestroyListenService()
    {
        if (event_base->EventThread())
        {
            if (ExistsStoped() || ExistsNoneed() || ExistsInited())
            {
                return true;
            }

            if (ExistsRuning())
            {
                if (0 == DeletedListenService())
                {
                    return false;
                }

                if (ChangeStatus(Status::Exec, Status::Stop))
                {
                    return event_thread_pool->DestroyEventThreadPool();
                }
            }
            return true;
        }
        return event_base->RunInLoopEx(std::bind<bool(TcpListen::*)(void)>(&TcpListen::DestroyListenService, this));
    }

    bool TcpListen::DestroyListenService(EventLoop* loop, socket_tcp* server)
    {
        if (nullptr != loop && nullptr != server && loop->EventBasic() == server->loop)
        {
            if (loop->EventThread())
            {
                return CloseedListenService(loop, server);
            }
            return loop->RunInLoopEx(std::bind<bool(TcpListen::*)(EventLoop*, socket_tcp*)>(&TcpListen::DestroyListenService, this, loop, server));
        }
        return false;
    }

    bool TcpListen::DeletedListenService()
    {
        for (u96 i = 0; i < tcp_server.size(); ++i)
        {
            if (DestroyListenService(event_thread_pool->GetEventLoop(i), tcp_server[i].get()))
            {
                continue;
            }
            return false;
        }

        if (event_close_flag_ex)
        {
            event_close_flag_ex.wait(1, std::memory_order_relaxed);
            {
                if (tcp_server.size())
                {
                    tcp_server.clear();
                    tcp_server.shrink_to_fit();

                }
            }
            return true;
        }
        return false;
    }

    bool TcpListen::CloseedListenService(EventLoop* loop, socket_tcp* server)
    {
        if (nullptr != loop)
        {
            uv_close(reinterpret_cast<event_handle*>(server), &TcpServer::OnDefaultListen);
            return true;
        }
        return false;
    }

    bool TcpListen::InitialListenService(EventSocketPool* socket, TcpServer* server, const u96 size)
    {
        if (nullptr != socket && nullptr != server)
        {
#ifdef H_OS_WINDOWS
            if (0 == InitEventThreadPools(size))
            {
                return false;
            }
#endif
            if (ChangeStatus(Status::None, Status::Init))
            {
                for (u96 i = 0; i < size; ++i)
                {
                    tcp_server.emplace(tcp_server.begin() + i, std::make_unique<socket_tcp>(std::move(socket_tcp{ .data = server })));
                    {
                        if (ExecuteListenService(event_thread_pool->GetEventLoop(i), tcp_server[i].get(), &socket->GetEventSocket(i)->GetSocketInfo()->addr, size, i))
                        {
                            EVENT_INFO("the server is starting, listening address: %s listening port: %u", socket->GetEventSocket(i)->GetSocketInfo()->host.c_str(), socket->GetEventSocket(i)->GetSocketInfo()->port);
                            continue;
                        }

                        return false;
                    }
                }

                event_start_flag.wait(1, std::memory_order_relaxed);

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

    bool TcpListen::ExecuteListenService(EventLoop* loop, socket_tcp* server, const sockaddr* addr, const u96 size, const u96 index)
    {
        if (loop->EventThread())
        {
            if (InitTcpService(loop, server))
            {
                if (uv_tcp_simultaneous_accepts(server, 0))
                {
                    return false;
                }

                if (tcp_proble)
                {
                    if (uv_tcp_nodelay(server, 1))
                    {
                        EVENT_INFO("an error occurred while setting the nodelay algorithm");
                    }
                }

                if (0 == BindTcpService(server, addr))
                {
                    return false;
                }

                if (0 == ListenTcpService(server))
                {
                    return false;
                }

                if (size == 1 + index)
                {
                    if (1 == event_start_flag.exchange(0, std::memory_order_release))
                    {
                        event_start_flag.notify_one();
                        return true;
                    }
                }
            }
            return false;
        }
        return loop->RunInLoop(std::bind(&TcpListen::ExecuteListenService, this, loop, server, addr, size, index));
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
        if (event_close_flag.fetch_add(1, std::memory_order_release) == tcp_server.size())
        {
            if (1 == event_close_flag_ex.exchange(0, std::memory_order_release))
            {
                event_close_flag_ex.notify_one();
                {
                    EVENT_INFO("the listening port has been closed");
                }
            }
        }
    }
}
