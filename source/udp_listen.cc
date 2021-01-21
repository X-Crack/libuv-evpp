#include <udp_listen.h>
#include <udp_server.h>
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

    static void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
    {
        static char slab[65536];
        buf->base = slab;
        buf->len = sizeof(slab);
    }


    void recv_cb(uv_udp_t* handle, ssize_t nread, const uv_buf_t* rcvbuf, const struct sockaddr* addr, unsigned flags) 
    {
        SocketInfo* socketinfo = reinterpret_cast<SocketInfo*>(const_cast<sockaddr*>(addr));
        socketinfo->SocketHash();
        // TODO; 计算 sockaddr hash 来判断是否为新连接
        if (nread < 0)
        {
            printf("?");
        }
        else if (nread == 0)
        {
            /* Returning unused buffer */
            printf("?");
        }
        else
        {
            printf("?");
        }
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
                        uv_udp_recv_start(udp_server[i].get(), alloc_cb, recv_cb);
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