#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__
#include <config.h>
namespace Evpp
{
    class EventLoop;
    class UdpServer
    {
    public:
        explicit UdpServer(EventLoop* loop);
        virtual ~UdpServer();
    public:

    private:
        EventLoop*                                                  event_loop;
    };
}
#endif // __UDP_SERVER_H__