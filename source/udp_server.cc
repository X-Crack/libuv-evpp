#include <udp_server.h>
#include <udp_listen.h>

#include <event_socket.h>
#include <event_socket_pool.h>

namespace Evpp
{
    UdpServer::UdpServer(EventLoop* loop) : 
        event_loop(loop),
        udp_listen(std::make_unique<UdpListen>(loop)),
        event_socket_pool(std::make_unique<EventSocketPool>())
    {
        
    }

    UdpServer::~UdpServer()
    {

    }

    bool UdpServer::CreaterServer()
    {
        if (nullptr != udp_listen)
        {
            return udp_listen->CreaterListenService(event_socket_pool);
        }
        return false;
    }

    bool UdpServer::AddListenPort(const std::string& server_address, const u16 port)
    {
        return event_socket_pool->AddListenPort(server_address, port);
    }


}