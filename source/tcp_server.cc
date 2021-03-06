﻿#include <tcp_server.h>
#include <tcp_listen.h>
#include <tcp_session.h>
#include <tcp_socket.h>
#include <event_share.h>
#include <event_socket.h>
#include <event_socket_pool.h>
#include <event_loop.h>
#include <event_loop_thread_pool.h>
#include <event_exception.h>
#include <event_coroutine.h>
namespace Evpp
{
    TcpServer::TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share) : TcpServer(loop, share, InterfaceAccepts(), InterfaceDiscons(), InterfaceMessage(), InterfaceSendMsg())
    {

    }

    TcpServer::TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share, const InterfaceAccepts& accepts, const InterfaceDiscons& discons, const InterfaceMessage& message, const InterfaceSendMsg& sendmsg) :
        event_base(loop),
        event_share(share),
        event_socket(std::make_unique<EventSocketPool>()),
        event_thread_pool(std::make_shared<EventLoopThreadPool>(loop, share)),
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
                    if (event_thread_pool->CreaterEventThreadPool(event_socket->GetSocketPoolSize()))
#endif
                    {
                        if (tcp_listen->CreaterListenService(event_socket.get(), this))
                        {
                            return ChangeStatus(Status::Init, Status::Exec);
                        }
                    }
                }
            }
        }
        return false;
    }

    bool TcpServer::DestroyServer()
    {
        if (nullptr != event_base)
        {
            if (ExistsRuning())
            {
                if (event_base->EventThread())
                {
                    if (DestroyService())
                    {
                        return ChangeStatus(Status::Stop, Status::Exit);
                    }
                    else
                    {
                        EVENT_INFO("an error occurred while destroying the server");
                    }
                    return false;
                }
                return RunInLoopEx(std::bind(&TcpServer::DestroyServer, this));
            }
            EVENT_INFO("server has stopped");
        }
        return false;
    }

    bool TcpServer::AddListenPort(const std::string& address, const u16 port)
    {
        if (ExistsStarts(Status::None))
        {
            if (nullptr != event_socket)
            {
                return event_socket->AddListenPort(address, port);
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
        return Send(GetSession(index), buf, len, nbufs);
    }

    bool TcpServer::Send(const u96 index, const std::string& buf, u32 nbufs)
    {
        if (tcp_session.empty())
        {
            return false;
        }
        return Send(GetSession(index), buf.c_str(), buf.size(), nbufs);
    }

    bool TcpServer::Send(TcpSession* session, const char* buf, u96 len, u32 nbufs)
    {
        if (nullptr != session)
        {
            return session->Send(buf, len, nbufs);
        }
        return false;
    }

    bool TcpServer::Close(const u96 index)
    {
        if (tcp_session.empty())
        {
            return false;
        }

        return Close(GetSession(index));
    }

    bool TcpServer::Close(TcpSession* session)
    {
        if (nullptr == session)
        {
            return false;
        }
        return session->Close();
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

    bool TcpServer::RunInLoop(const Handler& function)
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

    bool TcpServer::RunInQueue(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInQueue(function);
        }
        return false;
    }

    void TcpServer::DeletedSession(const u96 index)
    {
        {
            std::unique_lock<std::recursive_mutex> lock(tcp_recursive_mutex);
            if (tcp_session.size())
            {
                if (tcp_session.find(index) != tcp_session.end())
                {
                    tcp_socket->DelSockInfo(index);
                    tcp_session.erase(index);
                }
                tcp_index_multiplexing.emplace(index);
            }
        }

        if (ExistsStoped())
        {
            if (tcp_session.empty())
            {
                if (event_locking.notify())
                {
                    return;
                }
            }
        }
    }

    bool TcpServer::CleanedSession()
    {
        if (tcp_session.size())
        {
            {
                std::unique_lock<std::recursive_mutex> lock(tcp_recursive_mutex);
                for (auto& [index, session] : tcp_session)
                {
                    if (session->Close())
                    {
                        continue;
                    }
                }
            }
            return event_locking.dowait();
        }
        return true;
    }

    TcpSession* TcpServer::GetSession(const u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(tcp_recursive_mutex);
        if (tcp_session.find(index) != tcp_session.end())
        {
            return tcp_session[index].get();
        }
        return nullptr;
    }

    bool TcpServer::CreaterSession(EventLoop* loop, socket_tcp* client, const u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(tcp_recursive_mutex);
        return tcp_session.emplace
        (
            index,

            std::make_shared<TcpSession>
            (
                loop,
                client,
                index,
                std::bind(&TcpServer::DefaultDiscons, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                std::bind(&TcpServer::DefaultMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
                std::bind(&TcpServer::DefaultSendMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
                )
        ).second;
    }

    bool TcpServer::InitialSession(EventLoop* loop, socket_tcp* client, const u96 index)
    {
        if (nullptr != loop && nullptr != client)
        {
            if (loop->EventThread())
            {
                if (ExistsRuning())
                {
                    if (CreaterSession(loop, client, index))
                    {
                        if (tcp_socket->AddSockInfo(client, index))
                        {
                            if (nullptr != socket_accepts)
                            {
                                // messages cannot be sent asynchronously, because it is already asynchronous, and doing asynchronous is redundant, and it also involves the return value, whether to close the session.
                                if (socket_accepts(loop, tcp_session[index], index))
                                {
                                    return true;
                                }
                            }
                        }
                        return Close(index);
                    }
                }
                return SocketShutdown(client);
            }
            return loop->RunInQueue(std::bind(&TcpServer::InitialSession, this, loop, client, index));
        }
        return false;
    }

    bool TcpServer::InitialAccepts(EventLoop* loop, socket_stream* server, socket_tcp* client, const u96 index)
    {
        if (nullptr != loop && nullptr != server && nullptr != client)
        {
            if (ExistsRuning())
            {
                if (0 == uv_tcp_init(loop->EventBasic(), client))
                {
                    if (0 == uv_accept(server, reinterpret_cast<socket_stream*>(client)))
                    {
                        if (0 == uv_tcp_keepalive(client, 1, tcp_keepalive.load()))
                        {
                            return InitialSession(loop, client, index);
                        }
                    }
                }
            }

             if (0 == HandlerStatus(client))
             {
                 if (nullptr != client)
                 {
                     delete client;
                 }
                 return false;
             }

            return loop->RunInQueue(std::bind(&TcpServer::SocketShutdown, this, client));
        }
        return false;
    }

    bool TcpServer::DefaultAccepts(EventLoop* loop, socket_stream* server, socket_tcp* client, const u96 index)
    {
#ifdef H_OS_WINDOWS
        if (loop->EventThread())
        {
            if (nullptr != loop && nullptr != server && nullptr != client)
            {
#if defined(EVPP_USE_STL_COROUTINES)
                return JoinInTaskEx(std::bind(&TcpServer::InitialAccepts, this, loop, server, client, index)).get();
#else
                return InitialAccepts(loop, server, client, index);
#endif
            }
        }
        return loop->RunInLoopEx(std::bind<bool(TcpServer::*)(EventLoop*, socket_stream*, socket_tcp*, const u96)>(&TcpServer::DefaultAccepts, this, loop, server, client, index));
#elif H_OS_LINUX
        return loop->RunInQueue(std::bind(&TcpServer::InitialAccepts, this, loop, server, client, index));
#endif
    }

    bool TcpServer::DefaultAccepts(socket_stream* server, i32 status)
    {
        if (0 == status && nullptr != server)
        {
            u96 index = GetPlexingIndex();
            {
#ifdef H_OS_WINDOWS
                return DefaultAccepts(event_thread_pool->GetEventLoop(index), server, new socket_tcp(), index);
#else
                return DefaultAccepts(event_thread_pool->GetEventLoopEx(server->loop), server, new socket_tcp(), index);
#endif
            }
        }
        return false;
    }

    void TcpServer::DefaultDiscons(EventLoop* loop, socket_tcp* socket, const u96 index)
    {
        if (nullptr != loop)
        {
            if (nullptr != socket)
            {
                delete socket;
                socket = nullptr;
            }

            if (nullptr != socket_discons)
            {
                socket_discons(loop, index);
            }

            return DeletedSession(index);
        }
    }

    bool TcpServer::DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<EventBuffer>& buffer, const u96 index)
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

    bool TcpServer::SocketClose(socket_tcp* handler)
    {
        if (nullptr != handler)
        {
            return CloseHandler(handler, &TcpServer::OnDefaultClose);
        }
        return false;
    }

    bool TcpServer::SocketShutdown(socket_tcp* handler)
    {
        if (nullptr != handler)
        {
            return Evpp::SocketShutdown(handler, &TcpServer::OnDefaultShutdown);
        }
        return false;
    }

    const u96 TcpServer::GetPlexingIndex(u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(tcp_recursive_mutex);
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

    void TcpServer::OnDefaultClose(event_handle* handler)
    {
        if (nullptr != handler)
        {
            delete reinterpret_cast<socket_tcp*>(handler);
            handler = nullptr;
        }
    }

    void TcpServer::OnDefaultShutdown(socket_shutdown* shutdown, int status)
    {
        if (0 == status)
        {
            if (nullptr == shutdown || nullptr == shutdown->handle)
            {
                return;
            }

            if (CloseHandler(shutdown->handle, &TcpServer::OnDefaultClose))
            {
                if (nullptr != shutdown)
                {
                    delete shutdown;
                    shutdown = nullptr;
                }
            }
        }
    }

    void TcpServer::OnDefaultListen(event_handle* handler)
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

    bool TcpServer::DestroyService()
    {
        if (nullptr == tcp_listen || nullptr == event_thread_pool)
        {
            return false;
        }

        if (ChangeStatus(Status::Exec, Status::Stop))
        {
            if (tcp_listen->DestroyListenService())
            {
                // clean up the conversation list
                if (0 == CleanedSession())
                {
                    return false;
                }

                // destroy the thread pool
                if (0 == event_thread_pool->DestroyEventThreadPool())
                {
                    return false;
                }
                return true;
            }
        }
        return false;
    }
}
