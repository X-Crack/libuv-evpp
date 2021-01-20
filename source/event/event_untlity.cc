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
            printf("�û����� %d\n", index);
            return true;
        }

        void DefaultDiscons(EventLoop* loop, const u96 index)
        {
            (void)(loop); (void)(index);
            printf("�û��뿪 %d\n", index);
        }

        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index)
        {
            (void)(loop); (void)(session); (void)(buffer); (void)(index);

            printf("�û���Ϣ %d ��Ϣ���ȣ�%d\n", index, buffer->readableBytes());
            buffer->retrieve(buffer->readableBytes());
            return true;
        }
    }
}