#include <tcp_server.h>
#include <tcp_listen.h>
#include <tcp_session.h>
#include <event_share.h>
#include <event_socket.h>
#include <event_socket_pool.h>
#include <event_loop.h>
#include <event_loop_thread_pool.h>


namespace Evpp
{
    TcpServer::TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share) : TcpServer(loop, share, InterfaceAccepts(), InterfaceDiscons(), InterfaceMessage())
    {

    }

    TcpServer::TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share, const InterfaceAccepts& accepts, const InterfaceDiscons& discons, const InterfaceMessage& message) :
        event_base(loop),
        event_share(share),
        socket_accepts(accepts),
        socket_discons(discons),
        socket_message(message),
        event_thread_pool(std::make_shared<EventLoopThreadPool>(loop, share)),
        tcp_socket(std::make_unique<EventSocketPool>()),
#ifdef H_OS_WINDOWS
        tcp_listen(std::make_unique<TcpListen>(loop, true)),
#else
        tcp_listen(std::make_unique<TcpListen>(loop, event_thread_pool, true)),
#endif
        tcp_index(0)
    {

    }

    TcpServer::~TcpServer()
    {

    }

    bool TcpServer::CreaterServer(const u96 thread_size)
    {
        if (tcp_socket && tcp_listen)
        {
#ifdef H_OS_WINDOWS
            if (tcp_listen->CreaterListenService(tcp_socket.get(), this))
#else
            if (event_thread_pool->CreaterEventThreadPool(tcp_socket->GetSocketPoolSize()))
#endif
            {
                return event_thread_pool->CreaterEventThreadPool(thread_size);
            }
        }
        return false;
    }

    bool TcpServer::AddListenPort(const std::string& server_address, const u16 port)
    {
        if (nullptr != tcp_socket)
        {
            return tcp_socket->AddListenPort(server_address, port);
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

    bool TcpServer::DeletedSession(const u96 index)
    {
        std::lock_guard<std::recursive_mutex> lock(tcp_recursive_mutex);
        {
            if (auto it = tcp_session.find(index); it != std::end(tcp_session))
            {
                tcp_session.erase(it);
                tcp_index_multiplexing.emplace(index);
                return true;
            }
        }
        return false;
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
            std::bind(&TcpServer::DefaultMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
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
                    if (nullptr != socket_accepts)
                    {
                        return socket_accepts(loop, GetSession(index), index);
                    }
                    return true;
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
                    return true;
                }
            }
            return SystemShutdown(reinterpret_cast<socket_stream*>(client));
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
                    return InitialSession(loop, client);
                }
            }

            SystemClose(reinterpret_cast<socket_stream*>(client.get()));
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

    bool TcpServer::DefaultAccepts(socket_stream* server, i32 status)
    {
        if (0 == status && nullptr != server)
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
                    if (!watcher->DefaultAccepts(handler, status))
                    {
                        
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

                while (!DeletedSession(index));

                return;
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

    void TcpServer::SystemClose(socket_stream* stream)
    {
        if (CheckClose(stream))
        {
            return uv_close(reinterpret_cast<uv_handle_t*>(stream), &TcpServer::OnDefaultClose);
        }
        return;
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
}