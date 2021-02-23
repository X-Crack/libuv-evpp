#include <tcp_server_service.h>
#include <tcp_server.h>
#include <event_share.h>
#include <event_loop.h>
#include <event_timer.h>
#include <event_coroutine.h>
namespace Evpp
{
    TcpServerService::TcpServerService() :
        event_share(std::make_shared<EventShare>()),
        event_base(std::make_shared<EventLoop>(event_share->DefaultEventLoop())),
        tcp_server(std::make_unique<TcpServer>(event_base.get(), event_share))
    {

    }

    TcpServerService::~TcpServerService()
    {

    }

    bool TcpServerService::AddListenPort(const std::string& address, const u16 port)
    {
        if (nullptr != tcp_server)
        {
            return tcp_server->AddListenPort(address, port);
        }
        return false;
    }

    bool TcpServerService::RunInLoop(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoop(function);
        }
        return false;
    }

    bool TcpServerService::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoopEx(function);
        }
        return false;
    }

    bool TcpServerService::RunInQueue(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInQueue(function);
        }
        return false;
    }

    bool TcpServerService::CreaterServer(const u96 thread_size)
    {
        if (nullptr != event_share && nullptr != event_base && nullptr != tcp_server)
        {
            if (event_share->CreaterLoops(thread_size))
            {
                if (event_base->InitialEvent())
                {
                    return tcp_server->CreaterServer(thread_size);
                }
            }
        }
        return false;
    }

    bool TcpServerService::DestroyServer()
    {
        if (nullptr != tcp_server)
        {
            if (tcp_server->DestroyServer())
            {
                return event_base->StopDispatch();
            }
            else
            {
                assert(0);
            }
            return false;
        }
        return false;
    }

    bool TcpServerService::ExecDispatch(const i32 mode)
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatch(mode);
        }
        return false;
    }

    bool TcpServerService::ExecDispatch(const EventLoopHandler& function, i32 mode)
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatch(function, mode);
        }
        return false;
    }

    bool TcpServerService::ExecDispatchEx(const EventLoopHandler& function, i32 mode)
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatchEx(function, mode);
        }
        return false;
    }

    bool TcpServerService::ExecDispatchCoroutine(const EventLoopHandler& function, i32 mode)
    {
        if (nullptr != event_base)
        {
            for (; EventLoopAlive(event_base->EventBasic());)
            {
                try
                {
#if defined(EVPP_USE_STL_COROUTINES)
                    if (JoinInTaskEx(std::bind<bool(TcpServerService::*)(const EventLoopHandler&, i32)>(&TcpServerService::ExecDispatch, this, function, mode)).get())
                    {
                        continue; // 说明LOOP结束 跳到 for 判断一下是否有退出的条件
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

    bool TcpServerService::Send(const u96 index, const char* buf, u96 len, u32 nbufs)
    {
        if (nullptr != tcp_server)
        {
            return tcp_server->Send(index, buf, len, nbufs);
        }
        return false;
    }

    bool TcpServerService::Send(const u96 index, const std::string& buf, u32 nbufs)
    {
        if (nullptr != tcp_server)
        {
            return tcp_server->Send(index, buf, nbufs);
        }
        return false;
    }

    bool TcpServerService::Close(const u96 index)
    {
        if (nullptr != tcp_server)
        {
            return tcp_server->Close(index);
        }
        return false;
    }

    void TcpServerService::SetAcceptsCallback(const InterfaceAccepts& accepts)
    {
        if (nullptr != tcp_server)
        {
            return tcp_server->SetAcceptsCallback(accepts);
        }
    }

    void TcpServerService::SetDisconsCallback(const InterfaceDiscons& discons)
    {
        if (nullptr != tcp_server)
        {
            return tcp_server->SetDisconsCallback(discons);
        }
    }

    void TcpServerService::SetMessageCallback(const InterfaceMessage& message)
    {
        if (nullptr != tcp_server)
        {
            return tcp_server->SetMessageCallback(message);
        }
    }

    void TcpServerService::SetSendMsgCallback(const InterfaceSendMsg& sendmsg)
    {
        if (nullptr != tcp_server)
        {
            return tcp_server->SetSendMsgCallback(sendmsg);
        }
    }
}
