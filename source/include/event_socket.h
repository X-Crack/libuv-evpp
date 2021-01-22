#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__
#include <config.h>
#include <string>
#include <memory>
namespace Evpp
{
    struct SocketInfo
    {
    public:
        union
        {
            u16                 family;
            sockaddr            addr;
            sockaddr_in         addr4;
            sockaddr_in6        addr6;
        };
    public:
        std::string             host;
        u16                     port;
    public:
        constexpr const sockaddr* Addr()  { return &addr; };

        constexpr const sockaddr_in* Addr4() { return &addr4; };

        constexpr const sockaddr_in6* Addr6() { return &addr6; };

        constexpr const u96 SocketHash(const char* hash, u96 hash_mask, u32 i, u96 size)
        {
            if (i != size)
            {
                return SocketHash(++hash, hash_mask ^ (((i & 1) == 0) ? ((hash_mask << 7) ^ (*hash) * (hash_mask >> 3)) : (~((hash_mask << 11) + ((*hash) ^ (hash_mask >> 5))))), i, size);
            }
            return hash_mask;
        }

        const u96 SocketHash()
        {
            return SocketHash(reinterpret_cast<const char*>(Addr()), 2654435761U, 0, SocketSize());
        }

        constexpr const u32 SocketSize()
        {
            return SocketSize(family);
        }
    private:
        constexpr const u32 SocketSize(const u16 other)
        {
            return other == AF_INET ? offsetof(sockaddr_in, sin_zero) : sizeof(sockaddr_in6);
        }
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