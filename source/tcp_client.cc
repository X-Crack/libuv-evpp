#include <tcp_client.h>
#include <tcp_connect.h>
#include <tcp_session.h>
#include <event_socket.h>
#include <event_loop.h>
#include <event_timer.h>
namespace Evpp
{
    TcpClient::TcpClient(EventLoop* loop, const u96 index, const u32 reconnect) :
        event_loop(loop),
        safe_index(index),
        tcp_client(std::make_shared<socket_tcp>()),
        tcp_socket(std::make_unique<EventSocket>()),
        tcp_connect(std::make_unique<TcpConnect>(loop, tcp_client)),
        reconn_timer(std::make_shared<EventTimer>(loop, std::bind(&TcpClient::DefaultTimercb, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), index)),
        connect_mark(false),
        connect_tag(false),
        reconnect_after(reconnect),
        reconnect_delay(3000),
        reconnect_time(3000)
    {
        
    }

    TcpClient::~TcpClient()
    {

    }

    bool TcpClient::CreaterClient()
    {
        if (connect_mark.load())
        {
            return false;
        }
        else
        {
            connect_mark.store(true);
        }
        return ConnectService();
    }

    bool TcpClient::AddListenPort(const std::string& server_address, const u16 port)
    {
        return tcp_socket->CreaterSocket(server_address, port);
    }

    bool TcpClient::SetReconnect(const u32 reconnect)
    {
        return reconnect_after.compare_exchange_strong(reconnect_after._Storage._Value, reconnect);
    }

    void TcpClient::SetReconnectTimer(const u64 delay, const u64 time)
    {
        reconnect_delay.store(delay);
        reconnect_time.store(time);
    }

    void TcpClient::SetConnectCallback(const InterfaceConnect& connect)
    {
        if (nullptr == socket_connect_)
        {
            socket_connect_ = connect;
        }
    }

    void TcpClient::SetRestoreCallback(const InterfaceRestore& restore)
    {
        if (nullptr == socket_restore)
        {
            socket_restore = restore;
        }
    }

    void TcpClient::SetFailureCallback(const InterfaceFailure& failure)
    {
        if (nullptr == socket_failure)
        {
            socket_failure = failure;
        }
    }

    void TcpClient::SetDisconsCallback(const InterfaceDiscons& discons)
    {
        if (nullptr == socket_discons)
        {
            socket_discons = discons;
        }
    }

    void TcpClient::SetMessageCallback(const InterfaceMessage& message)
    {
        if (nullptr == socket_message)
        {
            socket_message = message;
        }
    }

    bool TcpClient::Close()
    {
        if (nullptr != tcp_session)
        {
            return tcp_session->Close();
        }
        return false;
    }

    bool TcpClient::InitialSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const u96 index)
    {
        if (nullptr == tcp_session)
        {
            tcp_session.reset(new TcpSession(
                loop,
                client,
                index,
                std::bind(&TcpClient::DefaultDiscons, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&TcpClient::DefaultMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
            ));
            return true;
        }
        return false;
    }

    bool TcpClient::DeletedSession()
    {
        if (nullptr != tcp_session)
        {
            tcp_session.reset();
            return true;
        }
        return false;
    }

    bool TcpClient::RemovedSession()
    {
        return DeletedSession() && ReConnectAfter(reconnect_delay.load(), reconnect_time.load());
    }

    bool TcpClient::ReConnectAfter(const u64 delay, const u64 time)
    {
        if (nullptr != reconn_timer)
        {
            return reconn_timer->AssignTimer(delay, time);
        }
        return false;
    }

    bool TcpClient::ConnectService()
    {
        if (tcp_socket && tcp_connect && nullptr == tcp_session)
        {
            return tcp_connect->ConnectServers(tcp_socket, this);
        }
        return false;
    }

    void TcpClient::DefaultConnect()
    {
        if (nullptr != event_loop)
        {
            if (InitialSession(event_loop, tcp_client, safe_index))
            {
                if (nullptr != tcp_session)
                {
                    if (connect_tag.load())
                    {
                        if (nullptr != socket_restore)
                        {
                            socket_restore(event_loop, tcp_session, safe_index);
                        }
                        return;
                    }

                    if (nullptr != socket_connect_)
                    {
                        socket_connect_(event_loop, tcp_session, safe_index);
                    }
                }
            }
        }
    }

    void TcpClient::DefaultFailure(int status)
    {
        String error_name[96];
        String error_msgs[96];
        {
            if (nullptr != socket_failure)
            {
                if (socket_failure(event_loop, safe_index, status, uv_err_name_r(status, error_name, 96), uv_strerror_r(status, error_msgs, 96)))
                {
                    if (reconnect_after.load())
                    {
                        if (connect_mark.exchange(false))
                        {
                            event_loop->RunInLoop(std::bind(&TcpClient::ReConnectAfter, this, reconnect_delay.load(), reconnect_time.load()));
                        }
                    }
                }
            }
        }
    }

    void TcpClient::DefaultDiscons(EventLoop* loop, const u96 index)
    {
        if (nullptr != socket_discons)
        {
            if (socket_discons(loop, index))
            {
                if (reconnect_after.load())
                {
                    if (connect_mark.exchange(false))
                    {
                        loop->RunInLoop(std::bind(&TcpClient::RemovedSession, this));
                    }
                }
            }
        }
    }

    bool TcpClient::DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index)
    {
        if (nullptr != socket_message)
        {
            return socket_message(loop, session, buffer, index);
        }
        return false;
    }

    void TcpClient::DefaultTimercb(EventLoop* loop, const std::shared_ptr<EventTimer>& timer, const u96 index)
    {
        if (nullptr != loop)
        {
            if (index == safe_index)
            {
                if (connect_mark.load())
                {
                    return;
                }

                if (nullptr != tcp_session)
                {
                    return;
                }

                if (ConnectService())
                {
                    if (timer->StopedTimer())
                    {
                        connect_tag.compare_exchange_strong(connect_tag._Storage._Value, true);
                        connect_mark.compare_exchange_strong(connect_mark._Storage._Value, true);
                    }
                }
            }
        }
    }

    void TcpClient::DefaultConnect(socket_connect* hanlder, int status)
    {
        if (nullptr != hanlder)
        {
            TcpClient* watcher = static_cast<TcpClient*>(hanlder->data);
            {
                if (nullptr != watcher)
                {
                    status ? watcher->DefaultFailure(status) : watcher->DefaultConnect();
                }
            }
        }
    }
}