#include <tcp_listen.h>
#include <tcp_server.h>
#include <event_share.h>
#include <event_loop.h>
#include <event_loop_thread_pool.h>
#include <event_socket.h>
#include <event_socket_pool.h>
#include <event_exception.h>
#include <event_mutex.h>
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
        if (ExistsStoped() || ExistsNoneed() || ExistsInited())
        {
            return true;
        }

        if (ExistsRuning())
        {
            if (0 == DeletedListenService())
            {
                EVENT_ERROR("an error occurred when stopping the monitoring service");
                return false;
            }
#ifdef H_OS_WINDOWS
            if (ChangeStatus(Status::Exec, Status::Stop))
            {
                return event_thread_pool->DestroyEventThreadPool();
            }
#else
            return ChangeStatus(Status::Exec, Status::Stop);
#endif
        }
        return false;
    }

    bool TcpListen::DestroyListenService(EventLoop* loop, socket_tcp* server, const u96 index)
    {
        if (nullptr != loop && nullptr != server)
        {
            if (loop->EventThread())
            {
                return CloseHandler(server, &TcpServer::OnDefaultListen);
            }
            return loop->RunInQueue(std::bind<bool(TcpListen::*)(EventLoop*, socket_tcp*, const u96)>(&TcpListen::DestroyListenService, this, loop, server, index)) && event_locking.dowait();
        }
        return false;
    }

    bool TcpListen::DeletedListenService()
    {
        for (u96 i = 0; i < tcp_server.size(); ++i)
        {
            if (DestroyListenService(event_thread_pool->GetEventLoop(i), tcp_server[i].get(), i))
            {
                continue;
            }
            return false;
        }
        return true;
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
                    tcp_server.emplace(tcp_server.begin() + i, std::make_unique<socket_tcp>(socket_tcp {server}));
                    {
                        if (ExecuteListenService(event_thread_pool->GetEventLoop(i), tcp_server[i].get(), &socket->GetEventSocket(i)->GetSocketInfo()->addr, i))
                        {
                            //EVENT_INFO("the server is starting, listening address: %s listening port: %u", socket->GetEventSocket(i)->GetSocketInfo()->host.c_str(), socket->GetEventSocket(i)->GetSocketInfo()->port);
                        }
                        else
                        {
                            EVENT_ERROR("an error occurred while creating the server");
                        }
                    }
                }
                return ChangeStatus(Status::Init, Status::Exec);
            }
        }
        return false;
    }
#ifdef H_OS_WINDOWS
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
#endif

    bool TcpListen::ExecuteListenService(EventLoop* loop, socket_tcp* server, const sockaddr* addr, const u96 index)
    {
        if (nullptr != loop && server != nullptr && addr != nullptr)
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
                            EVENT_FATAL("an error occurred while setting the nodelay algorithm");
                        }
                    }

                    if (0 == BindTcpService(server, addr))
                    {
                        EVENT_WARN("bind server port error");
                        return false;
                    }

                    if (0 == ListenTcpService(server))
                    {
                        EVENT_WARN("start listening error");
                        return false;
                    }

                    EVENT_INFO("Listen Thread Id: %d", loop->EventThreadId());

                    return true;
                }
                return false;
            }
            return loop->RunInLoopEx(std::bind(&TcpListen::ExecuteListenService, this, loop, server, addr, index));
        }
        return false;
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
        return ListenTcpServiceImpl(uv_listen(reinterpret_cast<socket_stream*>(server), SOMAXCONN, &TcpServer::OnDefaultAccepts));
    }

    bool TcpListen::ListenTcpServiceImpl(const i32 status)
    {
        if (0 == status)
        {
            return true;
        }

        String error_name[256];
        String error_msgs[256];
        EVENT_WARN("listen failed error code: %d error name:%s error message: %s", status, uv_err_name_r(status, error_name, std::size(error_name)), uv_strerror_r(status, error_msgs, std::size(error_msgs)));
        return false;

    }

    void TcpListen::OnClose()
    {
        event_locking.notify();
    }
}
