#include <udp_session.h>
#include <udp_message.h>
#include <event_loop.h>
namespace Evpp
{
    UdpSession::UdpSession(Eventloop* loop) : 
        event_loop(loop),
        udp_message(std::make_unique<UdpMessage>(loop))
    {

    }

    UdpSession::~UdpSession()
    {

    }

}