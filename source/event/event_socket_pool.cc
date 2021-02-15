#include <event_socket_pool.h>
#include <event_socket.h>
namespace Evpp
{
    EventSocketPool::EventSocketPool()
    {

    }

    EventSocketPool::~EventSocketPool()
    {

    }

    bool EventSocketPool::AddListenPort(const std::string& server_address, const u16 port)
    {
        if (CheckElements(server_address, port))
        {
            socket_bulder.push_back(std::make_shared<EventSocket>());
            {
                return socket_bulder.back()->CreaterSocket(server_address, port);
            }
        }
        return false;
    }

    const std::shared_ptr<EventSocket>& EventSocketPool::GetEventSocket(const u96 index)
    {
        return socket_bulder[index];
    }

    const u96 EventSocketPool::GetSocketPoolSize()
    {
        return socket_bulder.size();
    }

    bool EventSocketPool::CheckElements(const std::string& server_address, const u16 port)
    {
        for (u96 i = 0; i < socket_bulder.size(); ++i)
        {
            if (socket_bulder[i]->GetHostAddress() == server_address && socket_bulder[i]->GetHostPort() == port)
            {
                return false;
            }
        }
        return true;
    }
}
