#ifndef __UDP_MESSAGE_H__
#define __UDP_MESSAGE_H__
#include <config.h>
namespace Evpp
{
    class Eventloop;
    class UdpMessage
    {
    public:
        explicit UdpMessage(Eventloop* loop);
        virtual ~UdpMessage();
    public:
    private:
        Eventloop*                                              event_loop;
    };
}
#endif // __UDP_MESSAGE_H__