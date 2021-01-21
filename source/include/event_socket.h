#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__
#include <config.h>
#include <string>
#include <memory>
namespace Evpp
{
    struct SocketInfo
    {
        std::string             host;
        u16                     port;
        union
        {
            sockaddr            addr;
            sockaddr_in         addr4;
            sockaddr_in6        addr6;
        };
    };

    class EventSocket
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
    };
}
#endif // __TCP_SOCKET_H__