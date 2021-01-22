#ifndef __UDP_ACCEPT_H__
#define __UDP_ACCEPT_H__
#include <config.h>
namespace Evpp
{
    class UdpAccept
    {
    public:
        explicit UdpAccept();
        virtual ~UdpAccept();
    public:

    private:
        std::unique_ptr<socket_udp>                                             udp_socket;
    };
}

#endif // __UDP_ACCEPT_H__