#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__
#include <config.h>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class EventSocketPool;
    class UdpListen;
    class UdpServer
    {
    public:
        explicit UdpServer(EventLoop* loop);
        virtual ~UdpServer();
    public:
        bool CreaterServer();
        bool AddListenPort(const std::string& server_address, const u16 port);
    private:
        EventLoop*                                                  event_loop;
        std::unique_ptr<EventSocketPool>                            event_socket_pool;
        std::unique_ptr<UdpListen>                                  udp_listen;
    };
}
#endif // __UDP_SERVER_H__