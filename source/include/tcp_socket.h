#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__
#include <config.h>
#include <string>
#include <memory>
namespace evpp
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

    class TcpSocket
    {
    public:
        explicit TcpSocket();
        virtual ~TcpSocket();
    public:
        bool InitializeTcpSocket();
        bool AddListenPort(const std::string& server_address, const u16 port);
        u96  GetListeningPortSize();
    public:
        std::unique_ptr<SocketInfo>& GetSocketInfo(const u96 index);
    private:
        std::vector<std::unique_ptr<SocketInfo>>                                        socket_info;
    };
}
#endif // __TCP_SOCKET_H__