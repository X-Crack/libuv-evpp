#ifndef __TCP_SOCKET_POOL_H__
#define __TCP_SOCKET_POOL_H__
#include <event_config.h>
#include <memory>
#include <vector>
namespace Evpp
{
    class EventSocket;
    class EVPP_EXPORT EventSocketPool final
    {
    public:
        explicit EventSocketPool();
        virtual ~EventSocketPool();
    public:
        bool AddListenPort(const std::string& server_address, const u16 port);
        const std::shared_ptr<EventSocket>& GetEventSocket(const u96 index);
        const u96 GetSocketPoolSize();
    private:
        bool CheckElements(const std::string& server_address, const u16 port);
    private:
        std::vector<std::shared_ptr<EventSocket>>                      socket_bulder;
    };
}
#endif // __TCP_SOCKET_POOL_H__
