#ifndef __TCP_SERVER_SERVICE_H__
#define __TCP_SERVER_SERVICE_H__
#include <event_config.h>
#include <memory>
namespace Evpp
{
    class EventShare;
    class EventLoop;
    class EventTimer;
    class EventSynchronize;
    class EventSemaphore;
    class TcpServer;
    class EVPP_EXPORT TcpServerService
    {
    public:
        explicit TcpServerService();
        virtual ~TcpServerService();
    public:
        bool AddListenPort(const std::string& address, const u16 port);
    public:
        bool RunInLoop(const Handler& function);
        bool RunInLoopEx(const Handler& function);
        bool RunInQueue(const Handler& function);
    public:
        bool CreaterServer(const u96 thread_size);
        bool DestroyServer();
        bool ExecDispatch(const i32 mode = UV_RUN_DEFAULT);
        bool ExecDispatch(const EventLoopHandler& function, i32 mode = UV_RUN_ONCE);
        bool ExecDispatchEx(const EventLoopHandler& function, i32 mode = UV_RUN_NOWAIT);
        bool ExecDispatchCoroutine(const EventLoopHandler& function, i32 mode = UV_RUN_ONCE);
    public:
        bool Send(const u96 index, const char* buf, u96 len, u32 nbufs = 1);
        bool Send(const u96 index, const std::string& buf, u32 nbufs = 1);
        bool Close(const u96 index);
    public:
        void SetAcceptsCallback(const InterfaceAccepts& accepts = Import::DefaultAccepts);
        void SetDisconsCallback(const InterfaceDiscons& discons = Import::DefaultDiscons);
        void SetMessageCallback(const InterfaceMessage& message = Import::DefaultMessage);
        void SetSendMsgCallback(const InterfaceSendMsg& sendmsg = Import::DefaultSendMsg);
    private:
        std::shared_ptr<EventShare>                                     event_share;
        std::shared_ptr<EventLoop>                                      event_base;
        std::unique_ptr<TcpServer>                                      tcp_server;
        std::unique_ptr<EventSemaphore>                                 event_semaphore;
    };
}
#endif // __TCP_SERVER_SERVICE_H__
