﻿#include <tcp_connect.h>
#include <tcp_client.h>
#include <event_socket.h>
#include <event_loop.h>
#include <event_mutex.h>
namespace Evpp
{
    TcpConnect::TcpConnect(EventLoop* loop, socket_tcp* handler, TcpClient* client) :
        event_base(loop),
        tcp_handler(handler),
        tcp_connect(std::make_unique<socket_connect>())
    {
        if (nullptr == tcp_connect->data)
        {
            tcp_connect->data = client;
        }
    }

    TcpConnect::~TcpConnect()
    {

    }

    bool TcpConnect::ConnectService(const std::unique_ptr<EventSocket>& socket)
    {
        if (nullptr != socket)
        {
            if (InitTcpService())
            {
                if (uv_tcp_nodelay(tcp_handler, 1))
                {
                    printf("初始化失败\n");
                }

                return CreaterConnect(&socket->GetSocketInfo()->addr);
            }
        }
        return false;
    }

    bool TcpConnect::InitTcpService()
    {
        return 0 == uv_tcp_init(event_base->EventBasic(), tcp_handler);
    }

    bool TcpConnect::CreaterConnect(const sockaddr* addr)
    {
        return 0 == uv_tcp_connect(tcp_connect.get(), tcp_handler, addr, &TcpClient::DefaultConnect);
    }

    bool TcpConnect::DestroyConnect()
    {
        if (event_base->EventThread())
        {
            return CloseHandler(tcp_handler, 0);
        }
        return event_base->RunInLoopEx(std::bind(&TcpConnect::DestroyConnect, this));
    }
}
