#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include <config.h>
#include <memory>
#include <atomic>
namespace Evpp
{
    class EventLoop;
    class EventTimer;
    class EventSocket;
    class TcpConnect;
    class TcpSession;
    class TcpClient
    {
    public:
        explicit TcpClient(EventLoop* loop, const u96 index = 1, const u32 reconnect = 0);
        virtual ~TcpClient();
        friend TcpConnect;
    public:
        bool CreaterClient();
        bool AddListenPort(const std::string& server_address, const u16 port);
        bool SetReconnect(const u32 reconnect);
        void SetReconnectTimer(const u64 delay, const u64 time);
    public:
        void SetConnectCallback(const InterfaceConnect& connect = &Evpp::Import::DefaultConnect);
        void SetRestoreCallback(const InterfaceRestore& restore = &Evpp::Import::DefaultRestore);
        void SetFailureCallback(const InterfaceFailure& failure = &Evpp::Import::DefaultFailure);
        void SetDisconsCallback(const InterfaceDiscons& discons = &Evpp::Import::DefaultDiscons);
        void SetMessageCallback(const InterfaceMessage& message = &Evpp::Import::DefaultMessage);
        void SetSendMsgCallback(const InterfaceSendMsg& sendmsg);
    public:
        bool Close();
    private:
        bool InitialSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const u96 index);
        bool DeletedSession();
        bool RemovedSession();
    private:
        bool ReConnectAfter(const u64 delay, const u64 time);
        bool ConnectService();
    private:
        void DefaultConnect();
        void DefaultFailure(int status);
        void DefaultDiscons(EventLoop* loop, const u96 index);
        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index);
        bool DefaultSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status);
    private:
        void DefaultTimercb(EventLoop* loop, const std::shared_ptr<EventTimer>& timer, const u96 index);
    private:
        static void DefaultConnect(socket_connect* hanlder, int status);
    private:
        EventLoop*                                          event_base;
        u96                                                 self_index;
        InterfaceConnect                                    socket_connect_;
        InterfaceRestore                                    socket_restore;
        InterfaceFailure                                    socket_failure;
        InterfaceDiscons                                    socket_discons;
        InterfaceMessage                                    socket_message;
        InterfaceSendMsg                                    socket_sendmsg;
        std::shared_ptr<socket_tcp>                         tcp_client;
        std::unique_ptr<EventSocket>                        tcp_socket;
        std::unique_ptr<TcpConnect>                         tcp_connect;
        std::shared_ptr<TcpSession>                         tcp_session;
        std::shared_ptr<EventTimer>                         reconn_timer;
        std::atomic<bool>                                   connect_mark;
        std::atomic<bool>                                   connect_tag;
        std::atomic<u32>                                    reconnect_after;
        std::atomic<u64>                                    reconnect_delay;
        std::atomic<u64>                                    reconnect_time;
    };
}
#endif // __TCP_CLIENT_H__