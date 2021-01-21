#ifndef __UDP_LISTEN_H__
#define __UDP_LISTEN_H__
#include <config.h>
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
        bool SetMulticastInterface(const u96 index, const String* addr);
    private:
        EventLoop*                                              event_loop;
        std::vector<std::unique_ptr<socket_udp>>                udp_server;
    };
}
#endif // __UDP_LISTEN_H__