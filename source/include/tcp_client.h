#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include <config.h>
#include <event_status.h>
#include <memory>
#include <atomic>
namespace Evpp
{
    class EventLoop;
    class EventTimer;
    class EventSocket;
    class EventStatus;
    class TcpConnect;
    class TcpSession;
    class TcpAttach;
    class TcpClient : public EventStatus
    {
    public:
        explicit TcpClient(EventLoop* loop, const u96 index = 1);
        virtual ~TcpClient();
        friend TcpConnect;
        friend TcpAttach;
    public:
        bool CreaterClient();
        bool AddListenPort(const std::string& server_address, const u16 port);
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
        bool InitialSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const u96 index);
        bool DeletedSession();
    private:
        bool ConnectService();
    private:
        bool DefaultConnect();
        void DefaultFailure(int status);
        void DefaultDiscons(EventLoop* loop, const u96 index);
        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index);
        bool DefaultSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status);
    private:
        static void DefaultConnect(socket_connect* hanlder, int status);
    private:
        EventLoop*                                          event_base;
        u96                                                 event_index;
        InterfaceConnect                                    socket_connect_;
        InterfaceRestore                                    socket_restore;
        InterfaceFailure                                    socket_failure;
        InterfaceDiscons                                    socket_discons;
        InterfaceMessage                                    socket_message;
        InterfaceSendMsg                                    socket_sendmsg;
        std::shared_ptr<socket_tcp>                         socket_handler;
        std::unique_ptr<EventSocket>                        tcp_socket;
        std::unique_ptr<TcpConnect>                         tcp_connect;
        std::shared_ptr<TcpSession>                         tcp_session;
        std::unique_ptr<TcpAttach>                          tcp_attach;
        std::atomic<u32>                                    tcp_retry;
        std::atomic<u32>                                    tcp_retry_connection;
    };
}
#endif // __TCP_CLIENT_H__