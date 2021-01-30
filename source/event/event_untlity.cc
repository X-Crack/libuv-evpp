#include <event_untlity.h>
#include <tcp_session.h>
#include <event_loop.h>
#include <config.h>
#include <buffer.h>
namespace Evpp
{
    namespace Import
    {
        std::string send_data = "hello";
        bool DefaultAccepts(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index)
        {
            (void)loop, session, index;
            printf("DefaultAccepts:%d Thread:%d\n", index, loop->EventThreadId());
            return true; 
        }

        bool DefaultDiscons(EventLoop* loop, const u96 index)
        {
            (void)loop, index;
            printf("DefaultDiscons:%d Thread:%d\n", index, loop->EventThreadId());
            return true;
        }

        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index)
        {
            (void)loop, session, buffer, index;
            printf("Message:%d MessageLen:%d Thread:%d\n", index, buffer->readableBytes(), loop->EventThreadId());
            session->Send(send_data + std::to_string(index) + "\n" + char(10));
            buffer->retrieve(buffer->readableBytes());
            return true;
        }

        bool DefaultSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status)
        {
            printf("SendMessage:%d status:%d Thread:%d\n", index, status, loop->EventThreadId());
            return true;
        }

        bool DefaultConnect(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index)
        {
            (void)loop, session, index;
            return true;
        }

        bool DefaultRestore(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index)
        {
            (void)loop, session, index;
            printf("重新连接成功\n");
            return true;
        }

        bool DefaultFailure(EventLoop* loop, const u96 index, const i32 status, const String* name, const String* msgs)
        {
            (void)loop, index, status, name, msgs;
            printf("与服务器连接失败 - index:%d - status:%d - name:%s - msgs:%s\n", index, status, name, msgs);
            return true;
        }
    }
}