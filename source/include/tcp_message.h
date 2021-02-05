#ifndef __TCP_MESSAGE_H__
#define __TCP_MESSAGE_H__
#include <event_config.h>
#include <vector>
namespace Evpp
{
    class EventLoop;
    class EventBuffer;
    class TcpMessage
    {
    public:
        typedef std::function<void()>                                           SystemDiscons;
        typedef std::function<bool(const std::shared_ptr<EventBuffer>&)>        SystemMessage;
        typedef std::function<bool(const i32)>                                  SystemSendMsg;
    public:
        explicit TcpMessage(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const SystemDiscons& discons, const SystemMessage& message, const SystemSendMsg& sendmsg);
        virtual ~TcpMessage();
    public:
        bool RunInLoop(const Functor& function);
        bool RunInLoopEx(const Handler& function);
        bool Close();
        bool SetSendBlocking(const u32 value = 0);
        bool Send(const char* buf, u32 len, u32 nbufs = 1);
        bool Send(const std::string& buf, u32 nbufs = 1);
    private:
        bool DefaultSend(const socket_data bufs, u32 nbufs);
    private:
        bool CheckClose(socket_stream* handler);
        bool SystemShutdown(socket_stream* stream);
        bool SystemClose(socket_stream* stream);
    private:
        void OnSend(socket_write* request, int status);
        bool OnClose(event_handle* handler);
        void OnShutdown(socket_shutdown* shutdown, int status);
        void OnMallocEx(event_handle* handler, size_t suggested_size, socket_data* buf);
        bool OnMessages(socket_stream* stream, i96 nread, const socket_data* buf);
    private:
        static void DefaultSend(socket_write* handler, int status);
        static void DefaultClose(event_handle* handler);
        static void DefaultShutdown(socket_shutdown* handler, int status);
        static void DefaultMakesram(event_handle* handle, size_t suggested_size, socket_data* buf);
        static void DefaultMessages(socket_stream* handler, ssize_t nread, const socket_data* buf);
    private:
        EventLoop* event_base;
        SystemDiscons                                                   system_discons;
        SystemMessage                                                   system_message;
        SystemSendMsg                                                   system_sendmsg;
        std::shared_ptr<socket_tcp>                                     tcp_socket;
        std::shared_ptr<EventBuffer>                                    tcp_buffer;
        std::unique_ptr<socket_shutdown>                                event_shutdown;
        std::unique_ptr<socket_write>                                   event_write;
        std::vector<char>                                               event_data;
    };
}
#endif // __tcp_message_H__
