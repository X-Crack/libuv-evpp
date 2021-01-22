#ifndef __UDP_LISTEN_H__
#define __UDP_LISTEN_H__
#include <config.h>
#include <event_socket.h>
namespace Evpp
{
    class EventLoop;
    class EventSocketPool;
    class UdpListen
    {
    public:
        explicit UdpListen(EventLoop* loop);
        virtual ~UdpListen();
    public:
        bool CreaterListenService(const std::unique_ptr<EventSocketPool>& socket);
    private:
        bool InitUdpService(const u96 index);
        bool BindUdpService(const u96 index, const struct sockaddr* addr);
        bool ListenUdpService(const u96 index);
        bool SetMulticastInterface(const u96 index, const String* addr);
    private:
        bool AddClientObject(SocketInfo* socket, const u96 hash);
        bool GetClientObject(const u96 hash);
    private:
        void ReceiveService(socket_udp* handler, u96 nread, const socket_data* rcvbuf, struct SocketInfo* addr, u96 hash, u32 flags);
        static void ReceiveService(socket_udp* handler, ssize_t nread, const socket_data* rcvbuf, const sockaddr* addr, u32 flags);
    private:
        EventLoop*                                                              event_loop;
        std::vector<std::unique_ptr<socket_udp>>                                udp_server;
        std::unordered_map<u96, std::unique_ptr<SocketInfo>>                    udp_client;
    };
}
#endif // __UDP_LISTEN_H__