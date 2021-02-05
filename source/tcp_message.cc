﻿#include <tcp_message.h>
#include <event_loop.h>
#include <event_buffer.h>

namespace Evpp
{
    TcpMessage::TcpMessage(EventLoop* loop, const std::shared_ptr<socket_tcp>& client, const SystemDiscons& discons, const SystemMessage& message, const SystemSendMsg& sendmsg) :
        event_base(loop),
        system_discons(discons),
        system_message(message),
        system_sendmsg(sendmsg),
        tcp_socket(client),
        tcp_buffer(std::make_shared<EventBuffer>()),
        event_shutdown(std::make_unique<socket_shutdown>()),
        event_write(std::make_unique<socket_write>())
    {
        if (nullptr == client->data)
        {
            client->data = this;
        }

        if (nullptr == event_shutdown->data)
        {
            event_shutdown->data = this;
        }

        if (nullptr == event_write->data)
        {
            event_write->data = this;
        }

        if (uv_read_start(reinterpret_cast<socket_stream*>(client.get()), &TcpMessage::DefaultMakesram, &TcpMessage::DefaultMessages))
        {
            printf("init read error\n");
        }
    }

    TcpMessage::~TcpMessage()
    {

    }

    bool TcpMessage::RunInLoop(const Functor& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoop(function);
        }
        return false;
    }

    bool TcpMessage::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoopEx(function);
        }
        return false;
    }

    bool TcpMessage::Close()
    {
        if (nullptr != event_base)
        {
            if (event_base->EventThread())
            {
                if (nullptr != tcp_socket)
                {
                    return SystemClose(reinterpret_cast<socket_stream*>(tcp_socket.get()));
                }
                return false;
            }
            return RunInLoopEx(std::bind(&TcpMessage::Close, this));
        }
        return false;
    }

    bool TcpMessage::SetSendBlocking(const u32 value)
    {
        return 0 == uv_stream_set_blocking(reinterpret_cast<socket_stream*>(tcp_socket.get()), value);
    }

    bool TcpMessage::Send(const char* buf, u32 len, u32 nbufs)
    {
        if (nullptr != event_base)
        {
            if (event_base->EventThread())
            {
                if (len > 0 && buf)
                {
#ifdef H_OS_WINDOWS
                    return DefaultSend(socket_data{ len, const_cast<char*>(buf) }, nbufs);
#else
                    return DefaultSend(socket_data{ const_cast<char*>(buf), len }, nbufs);
#endif
                }
            }
            return RunInLoopEx(std::bind((bool(TcpMessage::*)(const char*, u32, u32)) & TcpMessage::Send, this, buf, len, nbufs));
        }
        return false;
    }

    bool TcpMessage::Send(const std::string& buf, u32 nbufs)
    {
        if (nullptr != event_base)
        {
            if (event_base->EventThread())
            {
                if (buf.capacity() > 0 && buf.data())
                {
#ifdef H_OS_WINDOWS
                    return DefaultSend(socket_data{ static_cast<u32>(buf.capacity()), const_cast<char*>(buf.data()) }, nbufs);
#else
                    return DefaultSend(socket_data{ const_cast<char*>(buf.data()), static_cast<u32>(buf.capacity()) }, nbufs);
#endif
                }
            }
            return RunInLoopEx(std::bind((bool(TcpMessage::*)(const std::string&, u32)) & TcpMessage::Send, this, buf, nbufs));
        }
        return false;
    }

    bool TcpMessage::DefaultSend(const socket_data bufs, u32 nbufs)
    {
        if (uv_is_active(reinterpret_cast<event_handle*>(tcp_socket.get())))
        {
            return 0 == uv_write(event_write.get(), reinterpret_cast<socket_stream*>(tcp_socket.get()), &bufs, nbufs, &TcpMessage::DefaultSend);
        }
        return false;
    }

    bool TcpMessage::CheckClose(socket_stream* handler)
    {
        if (nullptr != handler)
        {
            if (uv_is_active(reinterpret_cast<event_handle*>(handler)))
            {
                if (uv_is_closing(reinterpret_cast<event_handle*>(handler)))
                {
                    return 0 == uv_read_stop(reinterpret_cast<socket_stream*>(handler));
                }
            }
            return 0 == uv_read_stop(reinterpret_cast<socket_stream*>(handler));
        }
        return false;
    }

    bool TcpMessage::SystemShutdown(socket_stream* stream)
    {
        if (CheckClose(stream))
        {
            return 0 == uv_shutdown(event_shutdown.get(), stream, &TcpMessage::DefaultShutdown);
        }
        return false;
    }

    bool TcpMessage::SystemClose(socket_stream* stream)
    {
        if (CheckClose(stream))
        {
            uv_close(reinterpret_cast<event_handle*>(stream), &TcpMessage::DefaultClose);
            {
                return true;
            }
        }
        return false;
    }

    void TcpMessage::OnSend(socket_write* request, int status)
    {
        if (nullptr != request && 0 == status)
        {
            if (nullptr != system_sendmsg)
            {
                if (system_sendmsg(status))
                {
                    return;
                }

                if (SystemClose(reinterpret_cast<socket_stream*>(tcp_socket.get())))
                {
                    return;
                }

                printf("the client has been shut down\n");
            }
        }
    }

    bool TcpMessage::OnClose(event_handle* handler)
    {
        if (nullptr != handler)
        {
            if (nullptr != tcp_socket->data)
            {
                event_data.clear();
                event_data.shrink_to_fit();
                tcp_socket->data = nullptr;
            }

            if (nullptr != system_discons)
            {
                return RunInLoopEx(std::bind(system_discons));
            }
        }
    }

    void TcpMessage::OnShutdown(socket_shutdown* shutdown, int status)
    {
        if (0 == status)
        {
            if (nullptr != shutdown)
            {
                if (nullptr != shutdown->handle)
                {
                    uv_close(reinterpret_cast<event_handle*>(tcp_socket.get()), &TcpMessage::DefaultClose);
                }
            }
        }
    }

    void TcpMessage::OnMallocEx(event_handle* handler, size_t suggested_size, socket_data* buf)
    {
        if (nullptr != handler)
        {
            if (uv_is_active(handler))
            {
                if (event_data.capacity() != suggested_size)
                {
                    event_data.resize(suggested_size);
                }

                if (nullptr == buf->base)
                {
                    buf->base = event_data.data();
                    buf->len = static_cast<u32>(event_data.capacity());
                }
            }
        }
    }

    bool TcpMessage::OnMessages(socket_stream* stream, i96 nread, const socket_data* buf)
    {
        if (nread >= 0)
        {
            if (nullptr != tcp_buffer)
            {
                tcp_buffer->append(buf->base, nread);
                {
                    if (nullptr != system_message)
                    {
                        return system_message(std::weak_ptr<EventBuffer>(tcp_buffer).lock());
                    }
                }
                return true;
            }
            return false;
        }

        if (UV_ENOBUFS == nread)
        {
            if (event_data.capacity() < buf->len)
            {
                event_data.resize(buf->len);
            }

            return true;
        }

        if (UV_EOF == nread || UV_ECONNRESET == nread)
        {
            return SystemClose(stream);
        }
        return SystemShutdown(stream);
    }

    void TcpMessage::DefaultSend(socket_write* handler, int status)
    {
        if (nullptr != handler)
        {
            TcpMessage* watcher = static_cast<TcpMessage*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnSend(handler, status);
                }
            }
        }
    }

    void TcpMessage::DefaultClose(event_handle* handler)
    {
        if (nullptr != handler)
        {
            TcpMessage* watcher = static_cast<TcpMessage*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    if (watcher->OnClose(handler))
                    {
                        return;
                    }

                    printf("RunInLoop Error\n");
                }
            }
        }
    }

    void TcpMessage::DefaultShutdown(socket_shutdown* handler, int status)
    {
        if (nullptr != handler)
        {
            TcpMessage* watcher = static_cast<TcpMessage*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnShutdown(handler, status);
                }
            }
        }
    }

    void TcpMessage::DefaultMakesram(event_handle* handler, size_t suggested_size, socket_data* buf)
    {
        if (nullptr != handler)
        {
            TcpMessage* watcher = static_cast<TcpMessage*>(handler->data);
            {
                if (nullptr != watcher && 0 != suggested_size)
                {
                    watcher->OnMallocEx(handler, suggested_size, buf);
                }
            }
        }
    }

    void TcpMessage::DefaultMessages(socket_stream* handler, ssize_t nread, const socket_data* buf)
    {
        if (nullptr != handler)
        {
            TcpMessage* watcher = static_cast<TcpMessage*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    if (watcher->OnMessages(handler, static_cast<i96>(nread), buf))
                    {
                        return;
                    }

                    printf("OnSystemMessage Error\n");
                }
            }
        }
    }
}
