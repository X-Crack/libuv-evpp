#include <udp_message.h>

namespace Evpp
{
    UdpMessage::UdpMessage(Eventloop* loop) : 
        event_loop(loop)
    {
        
    }

    UdpMessage::~UdpMessage()
    {

    }
}