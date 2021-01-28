#ifndef __TCP_SERVER_SERVICE_H__
#define __TCP_SERVER_SERVICE_H__
#include <config.h>
#include <memory>
namespace Evpp
{
    class EventShare;
    class EventLoop;
    class EventTimer;
    class TcpServer;
    class TcpServerService
    {
    public:
        explicit TcpServerService();
        virtual ~TcpServerService();
    public:
        bool AddListenPort(const std::string& server_address, const u16 port);
    public:
        bool CreaterServer(const u96 thread_size);
        bool DestroyServer();
        bool DestroyServerEx();
        bool ExecDispatch();
        bool ExecDispatch(const u32 mode);
    public:
        bool Send(const u96 index, const char* buf, u96 len, u32 nbufs = 1);
        bool Send(const u96 index, const std::string& buf, u32 nbufs = 1);
        bool Close(const u96 index);
    public:
        void SetAcceptsCallback(const InterfaceAccepts& accepts);
        void SetDisconsCallback(const InterfaceDiscons& discons);
        void SetMessageCallback(const InterfaceMessage& message);
        void SetEventThreadId(const u32 id);
    private:
        std::shared_ptr<EventShare>                                     event_share;
        std::shared_ptr<EventLoop>                                      event_base;
        std::unique_ptr<TcpServer>                                      tcp_server;
    };
}
#endif // __TCP_SERVER_SERVICE_H__