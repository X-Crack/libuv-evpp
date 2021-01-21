#ifndef __UDP_SESSION_H__
#define __UDP_SESSION_H__
#include <config.h>
#include <memory>
namespace Evpp
{
    class Eventloop;
    class UdpMessage;
    class UdpSession
    {
    public:
        explicit UdpSession(Eventloop* loop);
        virtual ~UdpSession();
    public:
    private:
        Eventloop*                                      event_loop;
        std::unique_ptr<UdpMessage>                     udp_message;
    };
}
#endif // __UDP_SESSION_H__