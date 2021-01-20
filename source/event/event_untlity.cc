#include <event_untlity.h>
#include <config.h>
#include <buffer.h>
namespace Evpp
{
    namespace Import
    {
        bool DefaultAccepts(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index)
        {
            (void)(loop); (void)(session); (void)(index);
            printf("用户进入 %d\n", index);
            return true;
        }

        void DefaultDiscons(EventLoop* loop, const u96 index)
        {
            (void)(loop); (void)(index);
            printf("用户离开 %d\n", index);
        }

        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index)
        {
            (void)(loop); (void)(session); (void)(buffer); (void)(index);

            printf("用户消息 %d 消息长度：%d\n", index, buffer->readableBytes());
            buffer->retrieve(buffer->readableBytes());
            return true;
        }
    }
}