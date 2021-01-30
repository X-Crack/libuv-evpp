#include <tcp_client.h>
#include <tcp_attach.h>
#include <tcp_connect.h>
#include <tcp_session.h>
#include <event_socket.h>
#include <event_timer.h>
#include <event_loop.h>
namespace Evpp
{
    TcpClient::TcpClient(EventLoop* loop, const u96 index) :
        event_base(loop),
        event_index(index),
        socket_handler(std::make_shared<socket_tcp>()),
        tcp_socket(std::make_unique<EventSocket>()),
        tcp_connect(std::make_unique<TcpConnect>(loop, socket_handler, this)),
        tcp_attach(std::make_unique<TcpAttach>(loop, this)),
        tcp_retry(0),
        tcp_retry_connection(1),
        event_close_flag(0),
        event_close_flag_ex(0)
    {
        
    }

    TcpClient::~TcpClient()
    {

    }

    bool TcpClient::CreaterClient()
    {
        if (ExistsRuning() || ExistsStoped())
        {
            return false;
        }

        if (ExistsStarts(Status::None))
        {
            if (ChangeStatus(Status::None, Status::Init))
            {
                return ConnectService();
            }
        }
        return false;
    }

    bool TcpClient::DestroyClient(const bool wait)
    {
        if (nullptr != tcp_socket && nullptr != tcp_session)
        {
            if (ExistsStoped() || ExistsStarts(Status::None))
            {
                return true;
            }

            if (ChangeStatus(Status::Exec, Status::Exit))
            {
                if (tcp_retry_connection)
                {
                    // 关闭断线重连
                    tcp_retry_connection.store(0);
                }

                if (wait)
                {
                    if (RunInLoopEx(std::bind(&TcpClient::Close, this)))
                    {
                        std::atomic_wait_explicit(&event_close_flag, 0, std::memory_order_relaxed);
                    }
                    return true;
                }

                return Close();
            }
        }
        return false;
    }

    bool TcpClient::AddServerPort(const std::string& host, const u16 port)
    {
        return tcp_socket->CreaterSocket(host, port);
    }

    void TcpClient::SetResetConnectTimer(const u64 delay, const u64 timer)
    {
        return tcp_attach->SetResetConnectTimer(delay, timer);
    }

    void TcpClient::SetResetConnect(const u32 status)
    {
        tcp_retry_connection.store(status);
    }

    bool TcpClient::Send(const char* buf, u96 len, u32 nbufs)
    {
        if (nullptr != tcp_session)
        {
            return tcp_session->Send(buf, len, nbufs);
        }
        return false;
    }

    bool TcpClient::Send(const std::string& buf, u32 nbufs)
    {
        if (nullptr != tcp_session)
        {
            return tcp_session->Send(buf, nbufs);
        }
        return false;
    }

    bool TcpClient::Close()
    {
        if (nullptr != tcp_session)
        {
            if (ExistsRuning() || ExistsStarts(Status::Exit))
            {
                return tcp_session->Close();
            }
        }
        return false;
    }

    bool TcpClient::RunInLoop(const Functor& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoop(function);
        }
        return false;
    }

    bool TcpClient::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoopEx(function);
        }
        return false;
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

    void TcpClient::SetSendMsgCallback(const InterfaceSendMsg& sendmsg)
    {
        if (nullptr == socket_sendmsg)
        {
            socket_sendmsg = sendmsg;
        }
    }

    bool TcpClient::InitialSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const u96 index)
    {
        if (nullptr == tcp_session)
        {
            if (ChangeStatus(Status::Init, Status::Exec))
            {
                tcp_session.reset(new TcpSession
                (
                    loop,
                    client,
                    index,
                    std::bind(&TcpClient::DefaultDiscons, this, std::placeholders::_1, std::placeholders::_2),
                    std::bind(&TcpClient::DefaultMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
                    std::bind(&TcpClient::DefaultSendMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
                ));
                return true;
            }
        }
        return false;
    }

    bool TcpClient::DeletedSession()
    {
        if (nullptr != tcp_session)
        {
            // 已经存在连接对象，并且是正常被断开的状态。
            if (ChangeStatus(Status::Stop, Status::Init))
            {
                tcp_session.reset();
                {
                    if (tcp_retry_connection)
                    {
                        return tcp_attach->TryRetryConnect();
                    }
                }
            }

            // 已经存在连接对象，并且客户端主动销毁的状态。
            if (ExistsStarts(Status::Exit))
            {
                tcp_session.reset();
                event_close_flag.store(1, std::memory_order_release);
                std::atomic_notify_one(&event_close_flag);
            }
        }


        // 不存在连接对象，并且服务器无法连接，反复尝试状态。
        if (nullptr == tcp_session && ExistsStarts(Status::Init))
        {
            return tcp_attach->TryRetryConnect();
        }

        return false;
    }

    bool TcpClient::ConnectService()
    {
        if (tcp_socket && tcp_connect && nullptr == tcp_session)
        {
            if (event_base->EventThread())
            {
                return tcp_connect->ConnectService(tcp_socket);
            }
            return RunInLoopEx(std::bind(&TcpClient::ConnectService, this));
        }
        return false;
    }

    bool TcpClient::DefaultConnect()
    {
        if (nullptr != event_base)
        {
            if (InitialSession(event_base, socket_handler, event_index))
            {
                if (nullptr != tcp_session)
                {
                    if (tcp_retry_connection && tcp_retry)
                    {
                        if (nullptr != socket_restore)
                        {
                            if (socket_restore(event_base, tcp_session, event_index))
                            {
                                return true;
                            }
                            return false;
                        }
                    }

                    if (nullptr != socket_connect_)
                    {
                        if (socket_connect_(event_base, tcp_session, event_index))
                        {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    void TcpClient::DefaultFailure(int status)
    {
        String error_name[96];
        String error_msgs[96];
        {
            if (nullptr != socket_failure)
            {
                if (socket_failure(event_base, event_index, status, uv_err_name_r(status, error_name, std::size(error_name)), uv_strerror_r(status, error_msgs, std::size(error_msgs))))
                {
                    if (tcp_retry_connection)
                    {
                        if (RunInLoop(std::bind(&TcpClient::DeletedSession, this)))
                        {
                            return;
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
            if (ChangeStatus(Status::Exec, Status::Stop) || ExistsStarts(Status::Exit))
            {
                if (socket_discons(loop, index))
                {
                    if (RunInLoop(std::bind(&TcpClient::DeletedSession, this)))
                    {
                        return;
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

    bool TcpClient::DefaultSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status)
    {
        if (nullptr != socket_sendmsg)
        {
            return socket_sendmsg(loop, session, index, status);
        }
        return false;
    }

    void TcpClient::DefaultConnect(socket_connect* hanlder, int status)
    {
        if (nullptr != hanlder)
        {
            TcpClient* watcher = static_cast<TcpClient*>(hanlder->data);
            {
                if (nullptr != watcher)
                {
                    if (status)
                    {
                        watcher->DefaultFailure(status);
                        return;
                    }
                    
                    if (watcher->DefaultConnect())
                    {
                        return;
                    }

                    if (watcher->Close())
                    {
                        return;
                    }

                    printf("Close Client Error\n");
                }
            }
        }
    }
}