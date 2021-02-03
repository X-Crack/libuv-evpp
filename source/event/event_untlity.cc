#include <event_untlity.h>
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
            EVENT_INFO("DefaultAccepts:%d Thread:%d", index, loop->EventThreadId());
            return true;
        }

        bool DefaultDiscons(EventLoop* loop, const u96 index)
        {
            USE_PARAMETER(loop);
            USE_PARAMETER(index);
            EVENT_INFO("DefaultDiscons:%d Thread:%d", index, loop->EventThreadId());
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
            EVENT_INFO("SendMessage:%d status:%d Thread:%d\n", index, status, loop->EventThreadId());
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
}
