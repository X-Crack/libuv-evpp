#ifndef __TCP_CLIENT_SERVICE_H__
#define __TCP_CLIENT_SERVICE_H__
#include <config.h>
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
        bool AddListenPort(const std::string& host, const u16 port);
        void SetResetConnect(const u64 delay, const u64 timer);
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
    };
}
#endif // __TCP_CLIENT_SERVICE_H__