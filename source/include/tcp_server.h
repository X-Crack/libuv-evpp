#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
#include <config.h>
#include <memory>
#include <queue>
#include <mutex>
namespace evpp
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
        virtual ~TcpServer();
        friend TcpListen;
    public:
        bool CreaterServer();
        TcpServer& AddListenPort(const std::string& server_address, const u16 port);
    private:
        bool CreaterSession(EventLoop* loop, const std::shared_ptr<socket_tcp>& client);
    private:
        bool DefaultConnection(EventLoop* loop, socket_stream* handler);
        bool DefaultConnection(socket_stream* handler);
        static void DefaultConnection(socket_stream* handler, int status);
    private:
        void DefaultColseEx(socket_handle* handler);
        static void DefaultColse(socket_handle* handler);
    private:
        bool InitTcpSocket(EventLoop* loop, socket_stream* handler, socket_tcp* client);
    private:
        const u96 GetPlexingIndex();
    private:
        EventLoop*                                                      event_loop;
        std::shared_ptr<EventShare>                                     event_share;
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