#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__
#include <event_config.h>
#include <memory>
#include <unordered_map>
#include <mutex>
namespace Evpp
{
    class TcpSocket final
    {
    public:
        explicit TcpSocket();
        virtual ~TcpSocket();
    public:
        bool AddSockInfo(socket_tcp* handler, const u96 index);
        void DelSockInfo(const u96 index);
        SocketInfoEx* GetSockInfo(const u96 index);
    private:
        bool InitialSockInfo(socket_tcp* handler, SocketInfoEx* socket);
    private:
        bool GetSockInfo(socket_tcp* handler, SocketInfoEx* socket);
        bool GetPeerInfo(socket_tcp* handler, SocketInfoEx* socket);
    private:
        bool GetSockName(socket_tcp* handler, struct sockaddr* addr, i32 size);
        bool GetPeerName(socket_tcp* handler, struct sockaddr* addr, i32 size);
    private:
        std::unordered_map<u96, std::shared_ptr<SocketInfoEx>>                                          tcp_info;
        std::recursive_mutex                                                                            tcp_mutex;
    };
}
#endif // __TCP_SOCKET_H__