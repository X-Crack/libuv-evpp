#include <udp_listen.h>
#include <event_socket.h>
#include <event_socket_pool.h>
#include <event_loop.h>
namespace Evpp
{
    UdpListen::UdpListen(EventLoop* loop) : event_loop(loop)
    {

    }

    UdpListen::~UdpListen()
    {

    }

    bool UdpListen::CreaterListenService(const std::unique_ptr<EventSocketPool>& socket)
    {
        for (u96 i = 0; i < socket->GetSocketPoolSize(); ++i)
        {
            udp_server.push_back(std::make_unique<socket_udp>());
            {
                if (InitUdpService(i))
                {
                    if (BindUdpService(i, &socket->GetEventSocket(i)->GetSocketInfo()->addr))
                    {
                        if (!SetMulticastInterface(i, socket->GetEventSocket(i)->GetSocketInfo()->host.c_str()))
                        {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    bool UdpListen::InitUdpService(const u96 index)
    {
        return 0 == uv_udp_init(event_loop->EventBasic(), udp_server[index].get());
    }

    bool UdpListen::BindUdpService(const u96 index, const struct sockaddr* addr)
    {
        return 0 == uv_udp_bind(udp_server[index].get(), addr, 0);
    }

    bool UdpListen::SetMulticastInterface(const u96 index, const String* addr)
    {
        return 0 == uv_udp_set_multicast_interface(udp_server[index].get(), addr);
    }
}