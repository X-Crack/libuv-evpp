#ifndef __TCP_CLIENT_SERVICE_H__
#define __TCP_CLIENT_SERVICE_H__
#include <event_config.h>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class TcpClient;
    class TcpClientService
    {
    public:
        explicit TcpClientService(const u96 index = 1);
        virtual ~TcpClientService();
    public:
        bool CreaterClient();
        bool DestroyClient(const bool wait = true);
        bool AddServerPort(const std::string& host, const u16 port);
    public:
        bool ExecDispatch(i32 mode = UV_RUN_DEFAULT);
        // 回调用于客户端界面消息刷新防止窗口假死
        bool ExecDispatch(const EventLoopHandler& function, i32 mode = UV_RUN_NOWAIT);
        bool ExecDispatchEx(const EventLoopHandler& function, i32 mode = UV_RUN_ONCE);
        bool ExecDispatchCoroutine(const EventLoopHandler& function, i32 mode = UV_RUN_ONCE);
        bool StopDispatchEx();
    public:
        void SetResetConnectTimer(const u64 delay, const u64 timer);
        void SetResetConnect(const u32 status);
    public:
        bool Send(const char* buf, u96 len, u32 nbufs = 1);
        bool Send(const std::string& buf, u32 nbufs = 1);
    public:
        bool Close();
    public:
        bool RunInLoop(const Functor& function);
        bool RunInLoopEx(const Handler& function);
    public:
        void SetConnectCallback(const InterfaceConnect& connect = &Evpp::Import::DefaultConnect);
        void SetRestoreCallback(const InterfaceRestore& restore = &Evpp::Import::DefaultRestore);
        void SetFailureCallback(const InterfaceFailure& failure = &Evpp::Import::DefaultFailure);
        void SetDisconsCallback(const InterfaceDiscons& discons = &Evpp::Import::DefaultDiscons);
        void SetMessageCallback(const InterfaceMessage& message = &Evpp::Import::DefaultMessage);
        void SetSendMsgCallback(const InterfaceSendMsg& sendmsg = &Evpp::Import::DefaultSendMsg);
    private:
        std::shared_ptr<EventLoop>                      event_base;
        std::unique_ptr<TcpClient>                      tcp_client;
        std::atomic<u32>                                event_stop_flag;
    };
}
#endif // __TCP_CLIENT_SERVICE_H__
