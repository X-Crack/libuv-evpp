#include <tcp_server.h>
#include <tcp_listen.h>
#include <tcp_socket.h>
#include <tcp_session.h>

#include <event_share.h>
#include <event_loop.h>
#include <event_loop_thread_pool.h>

namespace evpp
{
    TcpServer::TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share) :
        event_loop(loop),
        event_share(share),
        event_thread_pool(std::make_unique<EventLoopThreadPool>(loop, share, share->GetLoopsSize())),
        tcp_socket(std::make_unique<TcpSocket>()),
        tcp_listen(std::make_unique<TcpListen>(loop, true)),
        tcp_index(0)
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

    // TODO: tcp_index_multiplexing.emplace()

    bool TcpServer::CreaterSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client)
    {
        std::lock_guard<std::mutex> lock(tcp_mutex);
        
        return tcp_session.emplace(GetPlexingIndex(), std::make_shared<TcpSession>(loop, client)).second;
    }

    bool TcpServer::DefaultConnection(EventLoop* loop, socket_stream* handler)
    {
        std::shared_ptr<socket_tcp> client = std::make_shared<socket_tcp>();
        {
            if (nullptr != event_thread_pool)
            {
                if (InitTcpSocket(loop, handler, client.get()))
                {
                    return CreaterSession(loop, client);
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

    void TcpServer::DefaultColseEx(socket_handle* handler)
    {
        (void)(handler);
    }

    void TcpServer::DefaultColse(socket_handle* handler)
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