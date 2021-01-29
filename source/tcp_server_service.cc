#include <tcp_server_service.h>
#include <event_share.h>
#include <event_loop.h>
#include <event_timer.h>
#include <tcp_server.h>
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

    bool TcpServerService::AddListenPort(const std::string& server_address, const u16 port)
    {
        if (nullptr != tcp_server)
        {
            return tcp_server->AddListenPort(server_address, port);
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
        return DestroyServerEx();
    }

    bool TcpServerService::DestroyServerEx()
    {
        if (nullptr != tcp_server)
        {
            return tcp_server->DestroyServer();
        }
        return false;
    }

    bool TcpServerService::ExecDispatch()
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatch();
        }
        return false;
    }

    bool TcpServerService::ExecDispatch(const u32 mode)
    {
        if (nullptr != event_base)
        {
            return event_base->ExecDispatch(mode);
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
}