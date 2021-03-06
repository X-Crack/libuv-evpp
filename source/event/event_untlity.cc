﻿#include <event_untlity.h>
#include <event_loop.h>
#include <event_buffer.h>
#include <tcp_session.h>
namespace Evpp
{
    namespace Import
    {
        std::string send_data = "hello";
        bool DefaultAccepts(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index)
        {
            USE_PARAMETER(loop);
            USE_PARAMETER(session);
            USE_PARAMETER(index);
            //EVENT_INFO("DefaultAccepts:%d Thread:%d", index, loop->EventThreadId());
            return true;
        }

        bool DefaultDiscons(EventLoop* loop, const u96 index)
        {
            USE_PARAMETER(loop);
            USE_PARAMETER(index);
            //EVENT_INFO("DefaultDiscons:%d Thread:%d", index, loop->EventThreadId());
            return true;
        }

        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<EventBuffer>& buffer, const u96 index)
        {
            USE_PARAMETER(loop);
            USE_PARAMETER(session);
            USE_PARAMETER(buffer);
            USE_PARAMETER(index);
            EVENT_INFO("receive client: %d message received length: %d thread: %d", index, buffer->readableBytes(), loop->EventThreadId());
            session->Send(send_data + std::to_string(index) + "\n" + char(10));
            buffer->retrieve(buffer->readableBytes());
            return true;
        }

        bool DefaultSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status)
        {
            USE_PARAMETER(loop);
            USE_PARAMETER(session);
            USE_PARAMETER(index);
            USE_PARAMETER(status);
            //EVENT_INFO("SendMessage:%d status:%d Thread:%d\n", index, status, loop->EventThreadId());
            return true;
        }

        bool DefaultConnect(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index)
        {
            USE_PARAMETER(loop);
            USE_PARAMETER(session);
            USE_PARAMETER(index);
            return true;
        }

        bool DefaultRestore(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index)
        {
            USE_PARAMETER(loop);
            USE_PARAMETER(session);
            USE_PARAMETER(index);
            EVENT_INFO("重新连接成功\n");
            return true;
        }

        bool DefaultFailure(EventLoop* loop, const u96 index, const i32 status, const String* name, const String* msgs)
        {
            USE_PARAMETER(loop);
            USE_PARAMETER(index);
            USE_PARAMETER(status);
            USE_PARAMETER(name);
            USE_PARAMETER(msgs);
            EVENT_INFO("与服务器连接失败 - index:%d - status:%d - name:%s - msgs:%s\n", index, status, name, msgs);
            return true;
        }
    }

    bool EventLoopAlive(event_loop* loop)
    {
        if (loop->active_handles == ~0U && loop->active_reqs.count == ~0U)
        {
            return false;
        }

        if (loop->active_handles > 0 || loop->active_reqs.count > 0)
        {
            return true;
        }

        return false;
    }
#ifdef H_OS_WINDOWS
    void DoDispatchEvent()
    {
        MSG msg;
        for (; PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);)
        {
            if (GetMessage(&msg, NULL, 0, 0))
            {
                if (TranslateMessage(&msg) && DispatchMessage(&msg))
                {
                    continue;
                }
            }
            break;
        }
    }
#endif

#ifdef H_OS_WINDOWS
    constexpr const u96 HashBit::Hash(const char* hash, const u96 size, u96 hash_mask, u32 index)
    {
        if (index != size)
        {
            return Hash(++hash, size, ~hash_mask ^ (!(index & 1) ? ((hash_mask << 7) ^ (*hash) * (hash_mask >> 3)) : (~((hash_mask << 11) + ((*hash) ^ (hash_mask >> 5))))), ++index);
        }
        return hash_mask ^ 0x7ffffff;
    }
#endif

#ifdef H_OS_WINDOWS
    bool SocketFormatErrorString(u32 code, String* format_string)
    {
        return FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), format_string, sizeof(*format_string), NULL);
    }
#endif
}
