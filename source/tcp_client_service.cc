﻿#include <tcp_client_service.h>
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

    bool TcpClientService::DestroyClient()
    {
        if (nullptr != tcp_client)
        {
            if (tcp_client->DestroyClient() && StopDispatch())
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

    bool TcpClientService::RunInLoop(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoop(function);
        }
        return false;
    }

    bool TcpClientService::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoopEx(function);
        }
        return false;
    }

    bool TcpClientService::RunInQueue(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInQueue(function);
        }
        return false;
    }

    bool TcpClientService::ExecDispatch(i32 mode)
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatch(mode);
        }
        return false;
    }

    bool TcpClientService::ExecDispatch(const EventLoopHandler& function, i32 mode)
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatch(function, mode);
        }
        return false;
    }

    bool TcpClientService::ExecDispatchEx(const EventLoopHandler& function, i32 mode)
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatchEx(function, mode);
        }
        return false;
    }

    bool TcpClientService::ExecDispatchCoroutine(const EventLoopHandler& function, i32 mode)
    {
        if (nullptr != event_base)
        {
            for (; event_stop_flag.load(std::memory_order_acquire);)
            {
                try
                {
#if defined(EVPP_USE_STL_COROUTINES)
                    if (JoinInTaskEx(std::bind<bool(TcpClientService::*)(const EventLoopHandler&, i32)>(&TcpClientService::ExecDispatch, this, function, mode)).get())
                    {
                        continue;
                    }
#else
                    if (ExecDispatch(function, mode))
                    {
                        continue;
                    }
#endif
                }
                catch (const EventRuntimeException& ex)
                {
                    EVENT_INFO("%s %d", ex.what(), ex.value());
                }
                catch (const EventException& ex)
                {
                    EVENT_INFO("during the operation of the coroutine there may be some problems please check carefully : %s", ex.what());
                }
                catch (...)
                {
                    EVENT_INFO("this is a exception without handling");
                }
            }
            return true;
        }
        return false;
    }

    bool TcpClientService::StopDispatch()
    {
        if (nullptr != event_base)
        {
            return event_base->StopDispatch();
        }
        return false;
    }

    void TcpClientService::SetResetConnectTimer(const u64 delay, const u64 timer)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetResetConnectTimer(delay, timer);
        }
    }

    void TcpClientService::SetResetConnect(const u32 status)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetResetConnect(status);
        }
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

    void TcpClientService::SetConnectCallback(const InterfaceConnect& connect)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetConnectCallback(connect);
        }
    }

    void TcpClientService::SetRestoreCallback(const InterfaceRestore& restore)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetRestoreCallback(restore);
        }
    }

    void TcpClientService::SetFailureCallback(const InterfaceFailure& failure)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetFailureCallback(failure);
        }
    }

    void TcpClientService::SetDisconsCallback(const InterfaceDiscons& discons)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetDisconsCallback(discons);
        }
    }

    void TcpClientService::SetMessageCallback(const InterfaceMessage& message)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetMessageCallback(message);
        }
    }

    void TcpClientService::SetSendMsgCallback(const InterfaceSendMsg& sendmsg)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetSendMsgCallback(sendmsg);
        }
    }
}
