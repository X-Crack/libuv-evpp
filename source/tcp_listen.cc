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
        return InitialListenService(socket, socket->GetSocketPoolSize(), server);
    }

    bool TcpListen::InitialListenService(EventSocketPool* socket, const u96 size, TcpServer* server)
    {
        // 存在bug 休息两天 打打LOL 在重写逻辑。
        if (InitialListenService(size))
        {
            for (u96 i = 0; i < size; ++i)
            {
                EventLoop* loop = event_thread_pool->GetEventLoop(i);
                {
                    if (!loop->EventThread())
                    {
                        return loop->RunInLoopEx(std::bind(&TcpListen::CreaterListenService, this, socket, server));
                    }

                    tcp_server.emplace(tcp_server.begin() + i, std::make_unique<socket_tcp>());
                    {
                        if (nullptr == tcp_server[i]->data)
                        {
                            tcp_server[i]->data = server;
                        }



                        if (InitTcpService(loop, i))
                        {
                            if (0 == uv_tcp_simultaneous_accepts(tcp_server[i].get(), 0))
                            {
                                if (tcp_proble)
                                {
                                    if (uv_tcp_nodelay(tcp_server[i].get(), 1))
                                    {
                                        printf("初始化失败\n");
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
            }
            return true;
        }
        return false;
    }

    bool TcpListen::InitialListenService(const u96 size)
    {
#ifdef H_OS_WINDOWS
        if (nullptr != event_share && nullptr != event_thread_pool)
        {
            if (event_share->CreaterLoops(size))
            {
                return event_thread_pool->CreaterEventThreadPool(size);
            }
        }
        return false;
#else
        return true;
#endif
    }

    bool TcpListen::InitTcpService(EventLoop* loop, const u96 index)
    {
        return 0 == uv_tcp_init(loop->EventBasic(), tcp_server[index].get());
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