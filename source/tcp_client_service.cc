#include <tcp_client_service.h>
#include <event_loop.h>
#include <tcp_client.h>
#include <event_coroutine.h>
namespace Evpp
{
    TcpClientService::TcpClientService(const u96 index) :
        event_base(std::make_shared<EventLoop>()),
        tcp_client(std::make_unique<TcpClient>(event_base.get(), index)),
        event_stop_flag(1)
    {

    }

    TcpClientService::~TcpClientService()
    {

    }

    bool TcpClientService::CreaterClient()
    {
        if (nullptr != tcp_client)
        {
            return event_base->InitialEvent() && tcp_client->CreaterClient();
        }
        return false;
    }

    bool TcpClientService::DestroyClient(const bool wait)
    {
        if (nullptr != tcp_client)
        {
            if (tcp_client->DestroyClient(wait) && StopDispatchEx())
            {
                if (event_stop_flag)
                {
                    event_stop_flag.store(0, std::memory_order_release);
                }
                return true;
            }
        }
        return false;
    }

    bool TcpClientService::AddServerPort(const std::string& host, const u16 port)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->AddServerPort(host, port);
        }
        return false;
    }

    bool TcpClientService::ExecDispatch()
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatch();
        }
    }

    bool TcpClientService::ExecDispatch(u32 mode)
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatch(mode);
        }
    }

    bool TcpClientService::ExecDispatch(const EventLoopHandler& function, u32 mode)
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatch(function, mode);
        }
    }

    bool TcpClientService::ExecDispatchEx(const EventLoopHandler& function, u32 mode)
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatchEx(function, mode);
        }
    }

    bool TcpClientService::ExecDispatchCoroutine(const EventLoopHandler& function, u32 mode)
    {
        if (nullptr != event_base)
        {
            for (; event_stop_flag.load(std::memory_order_acquire);)
            {
                try
                {
                    EventCoroutine::JoinInTask(std::bind((bool(TcpClientService::*)(const EventLoopHandler&, u32))&TcpClientService::ExecDispatch, this, function, mode));
                }
                catch (...)
                {
                	
                }
            }
            return true;
        }
        return false;
    }

    bool TcpClientService::StopDispatchEx()
    {
        if (nullptr != event_base)
        {
            return event_base->StopDispatchEx();
        }
    }

    void TcpClientService::SetResetConnectTimer(const u64 delay, const u64 timer)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetResetConnectTimer(delay, timer);
        }
        return;
    }

    void TcpClientService::SetResetConnect(const u32 status)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetResetConnect(status);
        }
        return;
    }

    bool TcpClientService::Send(const char* buf, u96 len, u32 nbufs)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->Send(buf, len, nbufs);
        }
        return false;
    }

    bool TcpClientService::Send(const std::string& buf, u32 nbufs)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->Send(buf, nbufs);
        }
        return false;
    }

    bool TcpClientService::Close()
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->Close();
        }
        return false;
    }

    bool TcpClientService::RunInLoop(const Functor& function)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->RunInLoop(function);
        }
        return false;
    }

    bool TcpClientService::RunInLoopEx(const Handler& function)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->RunInLoopEx(function);
        }
        return false;
    }

    void TcpClientService::SetConnectCallback(const InterfaceConnect& connect)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetConnectCallback(connect);
        }
        return;
    }

    void TcpClientService::SetRestoreCallback(const InterfaceRestore& restore)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetRestoreCallback(restore);
        }
        return;
    }

    void TcpClientService::SetFailureCallback(const InterfaceFailure& failure)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetFailureCallback(failure);
        }
        return;
    }

    void TcpClientService::SetDisconsCallback(const InterfaceDiscons& discons)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetDisconsCallback(discons);
        }
        return;
    }

    void TcpClientService::SetMessageCallback(const InterfaceMessage& message)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetMessageCallback(message);
        }
        return;
    }

    void TcpClientService::SetSendMsgCallback(const InterfaceSendMsg& sendmsg)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetSendMsgCallback(sendmsg);
        }
        return;
    }
}