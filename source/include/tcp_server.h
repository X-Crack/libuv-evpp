#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
#include <config.h>
#include <memory>
#include <queue>
#include <mutex>
namespace Evpp
{
    class EventLoop;
    class EventShare;
    class EventLoopThreadPool;
    class TcpSocket;
    class TcpListen;
    class TcpSession;
    class TcpServer
    {
    public:
        explicit TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share);
        explicit TcpServer(EventLoop* loop, const std::shared_ptr<EventShare>& share, const InterfaceAccepts& accepts, const InterfaceDiscons& discons, const InterfaceMessage& message);
        virtual ~TcpServer();
        friend TcpListen;
    public:
        bool CreaterServer();
        TcpServer& AddListenPort(const std::string& server_address, const u16 port);
    public:
        void SetAcceptsCallback(const InterfaceAccepts& accepts);
        void SetDisconsCallback(const InterfaceDiscons& discons);
        void SetMessageCallback(const InterfaceMessage& message);
    private:
        bool CreaterSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const u96 index);
        bool InitialSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client);
        bool DeletedSession(const u96 index);
        bool RemovedSession(const u96 index);
        const std::shared_ptr<TcpSession>& GetSession(const u96 index);
    private:
        bool DefaultAccepts(EventLoop* loop, socket_stream* handler);
        bool DefaultAccepts(socket_stream* handler);
        static void DefaultAccepts(socket_stream* handler, int status);
    private:
        void DefaultDiscons(EventLoop* loop, const u96 index);
        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index);
    private:
        bool InitTcpSocket(EventLoop* loop, socket_stream* handler, socket_tcp* client);
    private:
        const u96 GetPlexingIndex();
    private:
        EventLoop*                                                      event_loop;
        std::shared_ptr<EventShare>                                     event_share;
        InterfaceAccepts                                                socket_accepts;
        InterfaceDiscons                                                socket_discons;
        InterfaceMessage                                                socket_message;
        std::unique_ptr<EventLoopThreadPool>                            event_thread_pool;
        std::unique_ptr<TcpSocket>                                      tcp_socket;
        std::unique_ptr<TcpListen>                                      tcp_listen;
        std::atomic<u96>											    tcp_index;
        std::unordered_map<u96, std::shared_ptr<TcpSession>>            tcp_session;
        std::priority_queue<u96>									    tcp_index_multiplexing;
        std::mutex													    tcp_mutex;
    };
}
#endif // __TCP_SERVER_H__