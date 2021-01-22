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

    bool UdpListen::CreaterListenService(const std::unique_ptr<EventSocketPool>& socket)
    {
        for (u96 i = 0; i < socket->GetSocketPoolSize(); ++i)
        {
            udp_server.push_back(std::make_unique<socket_udp>());
            {
                if (nullptr == udp_server[i]->data)
                {
                    udp_server[i]->data = this;
                }

                if (InitUdpService(i))
                {
                    if (BindUdpService(i, &socket->GetEventSocket(i)->GetSocketInfo()->addr))
                    {
                        if (!SetMulticastInterface(i, socket->GetEventSocket(i)->GetSocketInfo()->host.c_str()))
                        {
                            return false;
                        }
                        
                        if (!ListenUdpService(i))
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

    bool UdpListen::ListenUdpService(const u96 index)
    {
        return 0 == uv_udp_recv_start(udp_server[index].get(), alloc, &UdpListen::ReceiveService);
    }

    bool UdpListen::SetMulticastInterface(const u96 index, const String* addr)
    {
        return 0 == uv_udp_set_multicast_interface(udp_server[index].get(), addr);
    }

    bool UdpListen::AddClientObject(SocketInfo* socket, const u96 hash)
    {
        return udp_client.emplace(hash, std::make_unique<SocketInfo>(std::move(*socket))).second;
    }

    bool UdpListen::GetClientObject(const u96 hash)
    {
        return udp_client.end() == udp_client.find(hash);
    }

    void UdpListen::ReceiveService(socket_udp* handler, u96 nread, const socket_data* rcvbuf, SocketInfo* addr, u96 hash, u32 flags)
    {
        if (GetClientObject(hash))
        {
            // 新用户进入
            if (AddClientObject(addr, hash))
            {
                // TODO: true;
            }
            // TODO: false;
            return;
        }

        if (nread > 0)
        {
            // 新消息
            return;
        }
        // Close
    }

    void UdpListen::ReceiveService(socket_udp* handler, ssize_t nread, const socket_data* rcvbuf, const sockaddr* addr, u32 flags)
    {
        if (nullptr != handler)
        {
            UdpListen* watcher = static_cast<UdpListen*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->ReceiveService(handler, nread, rcvbuf, reinterpret_cast<SocketInfo*>(const_cast<sockaddr*>(addr)), reinterpret_cast<SocketInfo*>(const_cast<sockaddr*>(addr))->SocketHash(), flags);
                }
            }
        }
    }
}