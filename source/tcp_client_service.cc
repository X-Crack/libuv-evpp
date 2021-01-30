#include <tcp_client_service.h>
#include <event_loop.h>
#include <tcp_client.h>
namespace Evpp
{
    TcpClientService::TcpClientService(const u96 index) :
        event_base(std::make_shared<EventLoop>()),
        tcp_client(std::make_unique<TcpClient>(event_base.get(), index))
    {

    }

    TcpClientService::~TcpClientService()
    {

    }

    bool TcpClientService::CreaterClient()
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->CreaterClient();
        }
        return false;
    }

    bool TcpClientService::AddListenPort(const std::string& host, const u16 port)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->AddListenPort(host, port);
        }
        return false;
    }

    void TcpClientService::SetResetConnect(const u64 delay, const u64 timer)
    {
        if (nullptr != tcp_client)
        {
            return tcp_client->SetResetConnect(delay, timer);
        }
        return;
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