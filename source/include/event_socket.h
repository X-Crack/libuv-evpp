#ifndef __EVENT_SOCKET_H__
#define __EVENT_SOCKET_H__
#include <config.h>
#include <string>
#include <memory>
namespace Evpp
{
    class EventSocket final
    {
    public:
        explicit EventSocket();
        virtual ~EventSocket();
    public:
        bool CreaterSocket(const std::string& host, const u16 port);
        const std::string& GetHostAddress();
        const u16 GetHostPort();
    private:
        bool InitialSocket();
    public:
        const std::unique_ptr<SocketInfo>& GetSocketInfo();
    private:
        std::unique_ptr<SocketInfo>                                        socket_t;
    };
}
#endif // __EVENT_SOCKET_H__