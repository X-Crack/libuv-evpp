#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__
#include <config.h>
#include <string>
#include <memory>
namespace Evpp
{
    struct SocketInfo final
    {
        union
        {
            u16                 family;
            sockaddr            addr;
            sockaddr_in         addr4;
            sockaddr_in6        addr6;
        };
        std::string             host;
        u16                     port;
    };

    class EventSocket final
    {
    public:
        explicit EventSocket();
        virtual ~EventSocket();
    public:
        bool CreaterSocket(const std::string& server_address, const u16 port);
        const std::string& GetHostAddress();
        const u16 GetHostPort();
    private:
        bool InitialSocket();
    public:
        const std::unique_ptr<SocketInfo>& GetSocketInfo();
    private:
        std::unique_ptr<SocketInfo>                                        socket_t;
        std::unordered_map<u96, u96> s;
    };
}
#endif // __TCP_SOCKET_H__