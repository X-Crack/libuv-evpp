#include <tcp_message.h>
#include <event_loop.h>

namespace Evpp
{
    TcpMessage::TcpMessage(EventLoop* loop, const std::shared_ptr<socket_tcp>& client) :
        event_loop(loop),
        tcp_socket(client)
    {
        if (nullptr == client->data)
        {
            client->data = this;
        }

        if (uv_read_start(reinterpret_cast<socket_stream*>(client.get()), &TcpMessage::DefaultMakesram, &TcpMessage::DefaultMessages))
        {
            printf("init read error\n");
        }
    }

    TcpMessage::~TcpMessage()
    {

    }

    bool TcpMessage::Send(const char* buf, u96 len, u32 nbufs)
    {
        if (len > 0 && buf)
        {
            return DefaultSend(socket_data{ len, const_cast<char*>(buf) }, nbufs);
        }
        return false;
    }

    bool TcpMessage::Send(const std::string& buf, u32 nbufs)
    {
        if (buf.capacity() > 0 && buf.data())
        {
            return DefaultSend(socket_data{ buf.capacity(), const_cast<char*>(buf.data()) }, nbufs);
        }
        return false;
    }

    bool TcpMessage::DefaultSend(const socket_data bufs, u32 nbufs)
    {
        if (uv_is_active(reinterpret_cast<event_handle*>(tcp_socket.get())))
        {
            return DefaultSend(&bufs, nbufs);
        }
        return false;
    }

    bool TcpMessage::DefaultSend(const socket_data* bufs, u32 nbufs)
    {
        if (event_loop->SelftyThread())
        {
            socket_write* request = new socket_write();
            {
                if (0 == request->data)
                {
                    request->data = this;
                }
                
                if (DefaultSend(request, reinterpret_cast<socket_stream*>(tcp_socket.get()), bufs, nbufs))
                {
                    if (rsend_data.data() == bufs->base)
                    {
                        delete bufs;
                        bufs = nullptr;
                    }
                    return true;
                }
            }
        }
        return event_loop->RunInLoop(std::bind((bool(TcpMessage::*)(const socket_data*, unsigned int))&TcpMessage::DefaultSend, this, CopyRequestData(bufs), nbufs));
    }

    bool TcpMessage::DefaultSend(socket_write* request, socket_stream* handler, const socket_data* bufs, unsigned int nbufs)
    {
        return 0 == uv_write(request, handler, bufs, nbufs, &TcpMessage::DefaultSend);
    }

    socket_data* TcpMessage::CopyRequestData(const socket_data* bufs)
    {
        if (nullptr != bufs && bufs->len > 0)
        {
            rsend_data.resize(bufs->len);
            {
                for (u96 i = 0; i < bufs->len; ++i)
                {
                    rsend_data[i] = bufs->base[i];
                }
            }
            return new socket_data{ bufs->len, rsend_data.data() };
        }
        return nullptr;
    }

    bool TcpMessage::CheckClose(socket_stream* handler)
    {
        if (nullptr != handler)
        {
            if (0 == uv_is_active(reinterpret_cast<event_handle*>(handler)))
            {
                if (0 == uv_is_closing(reinterpret_cast<event_handle*>(handler)))
                {
                    if (0 == uv_read_stop(reinterpret_cast<socket_stream*>(handler)))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool TcpMessage::Shutdown(socket_stream* stream, i96 nread)
    {
        if (nread == UV_EOF || nread == UV_ECONNRESET)
        {
            socket_shutdown* shutdown = new socket_shutdown();
            {
                if (nullptr == shutdown->data)
                {
                    shutdown->data = this;
                }

                if (CheckClose(stream))
                {
                    return 0 == uv_shutdown(shutdown, stream, &TcpMessage::DefaultShutdown);
                }
            }
        }
        return false;
    }

    void TcpMessage::OnSend(socket_write* request, int status)
    {
        if (nullptr != request && 0 == status)
        {
            delete request;
            request = nullptr;
        }
    }

    void TcpMessage::OnClose(event_handle* handle)
    {
        if (nullptr != handle)
        {
            event_data.clear();
            event_data.shrink_to_fit();
            rsend_data.clear();
            rsend_data.shrink_to_fit();
        }
    }

    void TcpMessage::OnShutdown(socket_shutdown* shutdown, int status)
    {
        if (nullptr != shutdown && 0 == status)
        {
            uv_close(reinterpret_cast<event_handle*>(shutdown->handle), &TcpMessage::DefaultClose);
            {
                delete shutdown;
                shutdown = nullptr;
            }
        }
    }

    void TcpMessage::OnMallocEx(event_handle* handler, size_t suggested_size, socket_data* buf)
    {
        if (nullptr != handler)
        {
            if (uv_is_active(handler))
            {
                if (event_data.capacity() != 65536)
                {
                    event_data.resize(suggested_size);
                }

                buf->base = event_data.data();
                buf->len = event_data.capacity();
            }
        }
    }

    bool TcpMessage::OnMessages(socket_stream* stream, i96 nread, const socket_data* buf)
    {
        (void)(buf);
        if (nread >= 0)
        {
            return true;
        }

        return Shutdown(stream, nread);
    }

    void TcpMessage::DefaultSend(socket_write* handler, int status)
    {
        TcpMessage* watcher = static_cast<TcpMessage*>(handler->data);
        {
            if (nullptr != watcher)
            {
                watcher->OnSend(handler, status);
            }
        }
    }

    void TcpMessage::DefaultClose(event_handle* handler)
    {
        TcpMessage* watcher = static_cast<TcpMessage*>(handler->data);
        {
            if (nullptr != watcher)
            {
                watcher->OnClose(handler);
            }
        }
    }

    void TcpMessage::DefaultShutdown(socket_shutdown* handler, int status)
    {
        TcpMessage* watcher = static_cast<TcpMessage*>(handler->data);
        {
            if (nullptr != watcher)
            {
                watcher->OnShutdown(handler, status);
            }
        }
    }

    void TcpMessage::DefaultMakesram(event_handle* handler, size_t suggested_size, socket_data* buf)
    {
        TcpMessage* watcher = static_cast<TcpMessage*>(handler->data);
        {
            if (nullptr != watcher && 0 != suggested_size)
            {
                watcher->OnMallocEx(handler, suggested_size, buf);
            }
        }
    }

    void TcpMessage::DefaultMessages(socket_stream* handler, ssize_t nread, const socket_data* buf)
    {
        TcpMessage* watcher = static_cast<TcpMessage*>(handler->data);
        {
            if (nullptr != watcher)
            {
                watcher->OnMessages(handler, static_cast<i96>(nread), buf);
            }
        }
    }
}