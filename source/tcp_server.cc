#include <tcp_server.h>
#include <tcp_listen.h>
#include <tcp_session.h>
#include <tcp_socket.h>
#include <event_share.h>
#include <event_socket.h>
#include <event_socket_pool.h>
#include <event_loop.h>
#include <event_loop_thread_pool.h>
namespace Evpp
{
    TcpServer::TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share) : TcpServer(loop, share, InterfaceAccepts(), InterfaceDiscons(), InterfaceMessage(), InterfaceSendMsg())
    {

    }

    TcpServer::TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share, const InterfaceAccepts& accepts, const InterfaceDiscons& discons, const InterfaceMessage& message, const InterfaceSendMsg& sendmsg) :
        event_base(loop),
        event_share(share),
        event_close_flag(1),
        event_close_flag_ex(1),
        event_thread_pool(std::make_shared<EventLoopThreadPool>(loop, share)),
        event_socket(std::make_unique<EventSocketPool>()),
        socket_accepts(accepts),
        socket_discons(discons),
        socket_message(message),
        socket_sendmsg(sendmsg),
        tcp_socket(std::make_unique<TcpSocket>()),
#ifdef H_OS_WINDOWS
        tcp_listen(std::make_unique<TcpListen>(loop, true)),
#else
        tcp_listen(std::make_unique<TcpListen>(loop, event_thread_pool, true)),
#endif
        tcp_index(0),
        tcp_keepalive(30)
    {

    }

    TcpServer::~TcpServer()
    {
        printf("Delete TcpServer\n");
    }

    bool TcpServer::CreaterServer(const u96 thread_size)
    {
        if (event_socket && tcp_listen)
        {
            if (ExistsStarts(Status::None))
            {
                if (ChangeStatus(Status::None, Status::Init))
                {
#ifdef H_OS_WINDOWS
                    if (event_thread_pool->CreaterEventThreadPool(thread_size))
#else
                    if (event_thread_pool->CreaterEventThreadPool(tcp_socket->GetSocketPoolSize()))
#endif
                    {
                        if (ChangeStatus(Status::Init, Status::Exec))
                        {
                            return tcp_listen->CreaterListenService(event_socket.get(), this);
                        }
                    }
                }
            }
        }
        return false;
    }

    bool TcpServer::DestroyServer(const bool wait)
    {
        if (nullptr != event_base)
        {
            if (event_base->EventThread())
            {
                if (ChangeStatus(Status::Exec, Status::Stop))
                {
                    if (tcp_listen->DestroyListenService())
                    {
                        if (CleanedSession())
                        {
                            if (event_thread_pool->DestroyEventThreadPool())
                            {
                                if (wait)
                                {
                                    event_close_flag_ex.store(0, std::memory_order_release);
                                    event_close_flag_ex.notify_one();
                                }

                                return true;
                            }
                        }
                    }
                }
                return false;
            }

            if (RunInLoopEx(std::bind(&TcpServer::DestroyServer, this, wait)))
            {
                if (wait)
                {
                    event_close_flag_ex.wait(1, std::memory_order_relaxed);
                }
                return true;
            }
        }
        return false;
    }

    bool TcpServer::AddListenPort(const std::string& server_address, const u16 port)
    {
        if (ExistsStarts(Status::None))
        {
            if (nullptr != event_socket)
            {
                return event_socket->AddListenPort(server_address, port);
            }
        }
        return false;
    }

    bool TcpServer::Send(const u96 index, const char* buf, u96 len, u32 nbufs)
    {
        if (tcp_session.empty())
        {
            return false;
        }
        return GetSession(index)->Send(buf, len, nbufs);
    }

    bool TcpServer::Send(const u96 index, const std::string& buf, u32 nbufs)
    {
        if (tcp_session.empty())
        {
            return false;
        }
        return GetSession(index)->Send(buf, nbufs);
    }

    bool TcpServer::Close(const u96 index)
    {
        if (tcp_session.empty())
        {
            return false;
        }

        return GetSession(index)->Close();
    }

    void TcpServer::SetKeepaLive(const u32 time)
    {
        tcp_keepalive.store(time);
    }

    u32  TcpServer::GetHardwareThreads()
    {
        return std::thread::hardware_concurrency();
    }

    void TcpServer::SetAcceptsCallback(const InterfaceAccepts& accepts)
    {
        if (nullptr == socket_accepts)
        {
            socket_accepts = accepts;
        }
    }

    void TcpServer::SetDisconsCallback(const InterfaceDiscons& discons)
    {
        if (nullptr == socket_discons)
        {
            socket_discons = discons;
        }
    }

    void TcpServer::SetMessageCallback(const InterfaceMessage& message)
    {
        if (nullptr == socket_message)
        {
            socket_message = message;
        }
    }

    void TcpServer::SetSendMsgCallback(const InterfaceSendMsg& sendmsg)
    {
        if (nullptr == socket_sendmsg)
        {
            socket_sendmsg = sendmsg;
        }
    }

    bool TcpServer::RunInLoop(const Functor& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoop(function);
        }
        return false;
    }

    bool TcpServer::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoopEx(function);
        }
        return false;
    }

    void TcpServer::DeletedSession(const u96 index)
    {
        if (tcp_session.size())
        {
            tcp_socket->DelSockInfo(index);
            {
                std::lock_guard<std::recursive_mutex> lock(tcp_recursive_mutex);
                tcp_session.erase(index);
                tcp_index_multiplexing.emplace(index);
            }
        }

        if (ExistsStoped())
        {
            if (tcp_session.empty())
            {
                event_close_flag.store(0, std::memory_order_release);
                event_close_flag.notify_one();
            }
        }
    }

    bool TcpServer::CleanedSession()
    {
        if (tcp_session.size())
        {
            {
                std::lock_guard<std::recursive_mutex> lock(tcp_recursive_mutex);
                for (auto & [index, session] : tcp_session)
                {
                    if (Close(index))
                    {
                        continue;
                    }
                    return false;
                }
            }

            event_close_flag.wait(1, std::memory_order_relaxed);
        }
        return tcp_session.empty();
    }

    const std::shared_ptr<TcpSession>& TcpServer::GetSession(const u96 index)
    {
        std::lock_guard<std::recursive_mutex> lock(tcp_recursive_mutex);
        return tcp_session[index];
    }


    bool TcpServer::CreaterSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const u96 index)
    {
        std::lock_guard<std::recursive_mutex> lock(tcp_recursive_mutex);
        return tcp_session.emplace
        (
            index, 

            std::make_shared<TcpSession>
            (
            loop,
            client,
            index,
            std::bind(&TcpServer::DefaultDiscons, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&TcpServer::DefaultMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
            std::bind(&TcpServer::DefaultSendMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
            )
        ).second;
    }

    bool TcpServer::InitialSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client)
    {
        if (loop->EventThread())
        {
            u96 index = GetPlexingIndex();
            {
                if (CreaterSession(loop, client, index))
                {
                    if (tcp_socket->AddSockInfo(client.get(), index))
                    {
                        if (nullptr != socket_accepts)
                        {
                            if (socket_accepts(loop, GetSession(index), index))
                            {
                                return true;
                            }

                            return Close(index);
                        }
                    }
                }
            }
            return SystemShutdown(reinterpret_cast<socket_stream*>(client.get()));
        }
        return loop->RunInLoopEx(std::bind(&TcpServer::InitialSession, this, loop, client));
    }

    bool TcpServer::InitTcpSocket(EventLoop* loop, socket_stream* handler, socket_tcp* client)
    {
        if (nullptr != loop && nullptr != client)
        {
            if (0 == uv_tcp_init(loop->EventBasic(), client))
            {
                if (0 == uv_accept(handler, reinterpret_cast<socket_stream*>(client)))
                {
                    return 0 == uv_tcp_keepalive(client, 1, tcp_keepalive.load());
                }

                if (SystemShutdown(reinterpret_cast<socket_stream*>(client)))
                {
                    return false;
                }
            }
        }
        return false;
    }

    bool TcpServer::DefaultAccepts(EventLoop* loop, socket_stream* server)
    {
        if (nullptr != event_thread_pool && nullptr != loop && nullptr != server)
        {
            std::shared_ptr<socket_tcp> client = std::make_shared<socket_tcp>();
            {
                if (InitTcpSocket(loop, server, client.get()))
                {
                    // 停止服务器不在接受新客户到来
                    if (ExistsRuning())
                    {
                        return InitialSession(loop, client);
                    }
                }

                return SystemClose(reinterpret_cast<socket_stream*>(client.get()));
            }
        }
        return false;
    }

    bool TcpServer::AsyncAccepts(EventLoop* loop, socket_stream* server)
    {
        if (nullptr != loop)
        {
            return loop->RunInLoopEx(std::bind((bool(TcpServer::*)(EventLoop*, socket_stream*))&TcpServer::DefaultAccepts, this, loop, server));
        }
        return false;
    }

    bool TcpServer::DefaultAccepts(socket_stream* server, i32 status_)
    {
        if (0 == status_ && nullptr != server)
        {
#ifdef H_OS_WINDOWS
            return DefaultAccepts(event_thread_pool->GetEventLoop(), server);
#else
            return AsyncAccepts(event_thread_pool->GetEventLoopEx(server->loop), server);
#endif
        }
        return false;
    }

    void TcpServer::OnDefaultAccepts(socket_stream* handler, int status)
    {
        if (nullptr != handler)
        {
            TcpServer* watcher = static_cast<TcpServer*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    if (watcher->DefaultAccepts(handler, status))
                    {
                        return;
                    }
                }
            }
        }
    }

    void TcpServer::DefaultDiscons(EventLoop* loop, const u96 index)
    {
        if (nullptr != loop)
        {
            if (loop->EventThread())
            {
                if (nullptr != socket_discons)
                {
                    socket_discons(loop, index);
                }

                return DeletedSession(index);
            }

            if (!loop->RunInLoopEx(std::bind(&TcpServer::DefaultDiscons, this, loop, index)))
            {
                printf("DefaultDiscons RunLoop Error\n");
            }
        }
    }

    bool TcpServer::DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index)
    {
        if (nullptr != loop && nullptr != socket_message)
        {
            return socket_message(loop, session, buffer, index);
        }
        return false;
    }

    bool TcpServer::DefaultSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status)
    {
        if (nullptr != loop && nullptr != socket_sendmsg)
        {
            return socket_sendmsg(loop, session, index, status);
        }
        return true;
    }

    bool TcpServer::CheckClose(socket_stream* handler)
    {
        if (nullptr != handler)
        {
            if (uv_is_active(reinterpret_cast<event_handle*>(handler)))
            {
                return 0 == uv_is_closing(reinterpret_cast<event_handle*>(handler));
            }
            return true;
        }
        return false;
    }

    bool TcpServer::SystemClose(socket_stream* stream)
    {
        if (CheckClose(stream))
        {
            uv_close(reinterpret_cast<uv_handle_t*>(stream), &TcpServer::OnDefaultClose);
            {
                return true;
            }
        }
        return false;
    }

    bool TcpServer::SystemShutdown(socket_stream* stream)
    {
        if (CheckClose(stream))
        {
            socket_shutdown* shutdown = new socket_shutdown();
            {
                if (nullptr == shutdown->data)
                {
                    shutdown->data = this;
                }

                return 0 == uv_shutdown(shutdown, stream, &TcpServer::OnDefaultShutdown);
            }
        }
        return false;
    }

    const u96 TcpServer::GetPlexingIndex(u96 index)
    {
        std::lock_guard<std::recursive_mutex> lock(tcp_recursive_mutex);
        {
            if (index == 0)
            {
                return GetPlexingIndex(tcp_index_multiplexing.empty() ? tcp_index.fetch_add(1) : tcp_index_multiplexing.top());
            }

            if (tcp_index_multiplexing.size())
            {
                tcp_index_multiplexing.pop();
            }
        }
        return index;
    }

    const u96 TcpServer::GetClientIndex()
    {
        return tcp_index.load();
    }

    void TcpServer::DefaultClose(event_handle* handler)
    {
        if (nullptr != handler->data)
        {
            handler->data = nullptr;
        }
    }

    void TcpServer::OnDefaultClose(event_handle* handler)
    {
        if (nullptr != handler)
        {
            TcpServer* watcher = static_cast<TcpServer*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->DefaultClose(handler);
                }
            }
        }
    }

    void TcpServer::OnDefaultShutdown(socket_shutdown* request, int status)
    {
        if (0 == status)
        {
            if (nullptr != request)
            {
                if (nullptr != request->handle)
                {
                    uv_close(reinterpret_cast<uv_handle_t*>(request->handle), &TcpServer::OnDefaultClose);
                }

                if (nullptr != request)
                {
                    delete request;
                    request = nullptr;
                }
            }
        }
    }

    void TcpServer::DefaultCloseListen(event_handle* handler)
    {
        if (nullptr != handler)
        {
            TcpServer* watcher = static_cast<TcpServer*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->tcp_listen->OnClose();
                }
            }
        }
    }
}