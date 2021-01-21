#include <tcp_client.h>
#include <tcp_connect.h>
#include <tcp_session.h>
#include <event_socket.h>
#include <event_loop.h>
#include <event_timer.h>
namespace Evpp
{
    TcpClient::TcpClient(EventLoop* loop, const u96 index) :
        event_loop(loop),
        safe_index(index),
        tcp_client(std::make_shared<socket_tcp>()),
        tcp_socket(std::make_unique<EventSocket>()),
        tcp_connect(std::make_unique<TcpConnect>(loop, tcp_client)),
        reconn_timer(std::make_shared<EventTimer>(loop, std::bind(&TcpClient::DefaultTimercb, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))),
        connect_mark(false)
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

    void TcpClient::SetConnectCallback(const InterfaceConnect& connect)
    {
        if (nullptr == socket_connect_)
        {
            socket_connect_ = connect;
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
        return DeletedSession() && ReConnectAfter(1000, 3000);
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
        if (InitialSession(event_loop, tcp_client, safe_index))
        {
            if (nullptr != tcp_session && nullptr != socket_connect_ && nullptr != event_loop)
            {
                socket_connect_(event_loop, tcp_session, safe_index);
            }
        }
    }

    void TcpClient::DefaultFailure(int status)
    {
        String error_msgs[96] = {};
        String error_name[96] = {};
        {
            if (nullptr != socket_failure)
            {
                if (socket_failure(event_loop, safe_index, status, error_name, error_msgs))
                {

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
                
            }
        }

        if (connect_mark.exchange(false))
        {
            loop->RunInLoop(std::bind(&TcpClient::RemovedSession, this));
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
        (void)index;
        if (nullptr != loop)
        {
            if (connect_mark.load())
            {
                return;
            }

            if (nullptr == tcp_session)
            {
                if (ConnectService())
                {
                    connect_mark.store(timer->StopedTimer());
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
                    status ? watcher->DefaultFailure(status)  : watcher->DefaultConnect();
                }
            }
        }
    }
}