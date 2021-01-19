#ifndef __TCP_MESSAGE_H__
#define __TCP_MESSAGE_H__
#include <config.h>
#include <vector>
namespace Evpp
{
    class EventLoop;
    class TcpBuffer;
    class TcpMessage
    {
    public:
        explicit TcpMessage(EventLoop* loop, const std::shared_ptr<socket_tcp>& client);
        virtual ~TcpMessage();
    public:
        bool Send(const char* buf, u96 len, u32 nbufs = 1);
        bool Send(const std::string& buf, u32 nbufs = 1);
    private:
        bool DefaultSend(const socket_data bufs, u32 nbufs);
        bool DefaultSend(const socket_data* bufs, u32 nbufs);
        bool DefaultSend(socket_write* request, socket_stream* handler, const socket_data* bufs, unsigned int nbufs);
        socket_data* CopyRequestData(const socket_data* bufs);
    private:
        bool CheckClose(socket_stream* handler);
        bool Shutdown(socket_stream* stream, i96 nread);
    private:
        void OnSend(socket_write* request, int status);
        void OnClose(event_handle* handler);
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
        EventLoop*                                                      event_loop;
        std::shared_ptr<socket_tcp>                                     tcp_socket;
        std::shared_ptr<TcpBuffer>                                      tcp_buffer;
        std::vector<char>                                               event_data;
        std::vector<char>                                               rsend_data;
    };
}
#endif // __tcp_message_H__