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
        constexpr const sockaddr* Addr() const  { return &addr; };

        constexpr const sockaddr_in* Addr4() { return &addr4; };

        constexpr const sockaddr_in6* Addr6() { return &addr6; };

        constexpr const u96 SocketHash(const char* hash, const u96 size, const u96 hash_mask, u32 i) const
        {
            if (i != size)
            {
                return SocketHash(++hash, size, ~hash_mask ^ (!(i & 1) ? ((hash_mask << 7) ^ (*hash) * (hash_mask >> 3)) : (~((hash_mask << 11) + ((*hash) ^ (hash_mask >> 5))))), ++i);
            }
            return hash_mask ^ 0x7ffffff;
        }

        const u96 SocketHash()
        {
            return SocketHash(reinterpret_cast<const char*>(Addr()), SocketSize(), 2654435761U, 0);
        }

        const u96 SocketHash() const
        {
            return SocketHash(reinterpret_cast<const char*>(Addr()), SocketSize(), 2654435761U, 0);
        }

        constexpr const u32 SocketSize() const
        {
            return SocketSize(family);
        }

        bool EqualTo(const SocketInfo* other) const
        {
            return memcmp(this, other, SocketSize());
        }
    private:
        constexpr const u32 SocketSize(const u16 other) const
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
        std::unordered_map<u96, u96> s;
    };
}
#endif // __TCP_SOCKET_H__