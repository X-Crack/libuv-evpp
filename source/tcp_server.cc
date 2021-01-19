#include <tcp_server.h>
#include <tcp_listen.h>
#include <tcp_socket.h>
#include <tcp_session.h>

#include <event_share.h>
#include <event_loop.h>
#include <event_loop_thread_pool.h>

namespace Evpp
{
    TcpServer::TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share) : TcpServer(loop, share, InterfaceAccepts(), InterfaceDiscons(), InterfaceMessage())
    {

    }

    TcpServer::TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share, const InterfaceAccepts& accepts, const InterfaceDiscons& discons, const InterfaceMessage& message) :
        event_loop(loop),
        event_share(share),
        event_thread_pool(std::make_unique<EventLoopThreadPool>(loop, share, share->GetLoopsSize())),
        tcp_socket(std::make_unique<TcpSocket>()),
        tcp_listen(std::make_unique<TcpListen>(loop, true)),
        tcp_index(0),
        socket_accepts(accepts),
        socket_discons(discons),
        socket_message(message)
    {

    }

    TcpServer::~TcpServer()
    {

    }

    bool TcpServer::CreaterServer()
    {
        if (tcp_socket && tcp_socket->InitializeTcpSocket() && tcp_listen)
        {
            if (event_thread_pool->CreaterEventThreadPool() && event_thread_pool->InitialEventThreadPool())
            {
                return tcp_listen->CreaterListenService(tcp_socket, this);
            }
        }
        return false;
    }

    TcpServer& TcpServer::AddListenPort(const std::string& server_address, const u16 port)
    {
        if (false == tcp_socket->AddListenPort(server_address, port))
        {
            printf("server address is not\n");
        }
        return *this;
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

    bool TcpServer::CreaterSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const u96 index)
    {
        std::lock_guard<std::mutex> lock(tcp_mutex);
        
        return tcp_session.emplace(index, std::make_shared<TcpSession>(loop, 
            client, 
            index, 
            std::bind(&TcpServer::DefaultDiscons, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&TcpServer::DefaultMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
            )).second;
    }

    bool TcpServer::InitialSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client)
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
    }

    bool TcpServer::DeletedSession(const u96 index)
    {
        std::lock_guard<std::mutex> lock(tcp_mutex);
        if (auto it = tcp_session.find(index); it != std::end(tcp_session))
        {
            tcp_session.erase(it);
            tcp_index_multiplexing.emplace(index);
            return true;
        }
        return false;
    }

    bool TcpServer::RemovedSession(const u96 index)
    {
        return DeletedSession(index);
    }

    const std::shared_ptr<TcpSession>& TcpServer::GetSession(const u96 index)
    {
        return tcp_session[index];
    }

    bool TcpServer::DefaultConnection(EventLoop* loop, socket_stream* handler)
    {
        std::shared_ptr<socket_tcp> client = std::make_shared<socket_tcp>();
        {
            if (nullptr != event_thread_pool)
            {
                if (InitTcpSocket(loop, handler, client.get()))
                {
                    return InitialSession(loop, client);
                }
            }
        }
        return false;
    }

    bool TcpServer::DefaultConnection(socket_stream* handler)
    {
        EventLoop* loop = event_thread_pool->GetEventLoop();
        {
            if (nullptr != loop)
            {
                if (loop->SelftyThread())
                {
                    return DefaultConnection(loop, handler);
                }
                return loop->RunInLoop(std::bind((bool(TcpServer::*)(EventLoop*, socket_stream*))&TcpServer::DefaultConnection, this, loop, handler));
            }
        }
        return false;
    }

    void TcpServer::DefaultConnection(socket_stream* handler, int status)
    {
        if (status < 0)
        {
            printf(" new connection error\n");
            return;
        }

        TcpServer* watcher = static_cast<TcpServer*>(handler->data);
        {
            if (nullptr != watcher)
            {
                watcher->DefaultConnection(handler);
            }
        }
    }

    void TcpServer::DefaultDiscons(EventLoop* loop, const u96 index)
    {
        if (nullptr != loop)
        {
            if (loop->SelftyThread())
            {
                if (RemovedSession(index))
                {
                    if (nullptr != socket_discons)
                    {
                        socket_discons(loop, index);
                    }
                }
                return;
            }

            loop->RunInLoop(std::bind(&TcpServer::RemovedSession, this, index));
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

    void TcpServer::DefaultColseEx(event_handle* handler)
    {
        (void)(handler);
    }

    void TcpServer::DefaultColse(event_handle* handler)
    {
        TcpServer* watcher = static_cast<TcpServer*>(handler->data);
        {
            if (nullptr != watcher)
            {
                watcher->DefaultColseEx(handler);
            }
        }
    }

    bool TcpServer::InitTcpSocket(EventLoop* loop, socket_stream* handler, socket_tcp* client)
    {
        if (nullptr != loop && nullptr != client)
        {
            if (0 == uv_tcp_init(loop->EventBasic(), client) && 0 == uv_accept(handler, reinterpret_cast<socket_stream*>(client)))
            {
                return true;
            }

            uv_close(reinterpret_cast<uv_handle_t*>(handler), 0);
        }
        return false;
    }

    const u96 TcpServer::GetPlexingIndex()
    {
        std::lock_guard<std::mutex> lock(tcp_mutex);
        const u96 index = tcp_index_multiplexing.empty() ? tcp_index.fetch_add(1) : tcp_index_multiplexing.top();
        {
            if (tcp_index_multiplexing.size())
            {
                tcp_index_multiplexing.pop();
            }
        }
        return index;
    }
}