#include <event_untlity.h>
#include <tcp_session.h>
#include <config.h>
#include <buffer.h>
namespace Evpp
{
    namespace Import
    {
        std::string send_data = "test";
        bool DefaultAccepts(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index)
        {
            (void)loop, session, index;
            //printf("�û����� %d\n", index);
            return true; 
        }

        bool DefaultDiscons(EventLoop* loop, const u96 index)
        {
            (void)loop, index;
            //printf("�û��뿪 %d\n", index);
            return true;
        }

        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index)
        {
            (void)loop, session, buffer, index;
            //printf("�û���Ϣ %d ��Ϣ���ȣ�%d\n", index, buffer->readableBytes());
            //buffer->retrieve(buffer->readableBytes());
            session->Send(send_data);
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
            printf("�������ӳɹ�\n");
            return true;
        }

        bool DefaultFailure(EventLoop* loop, const u96 index, const i32 status, const String* name, const String* msgs)
        {
            (void)loop, index, status, name, msgs;
            printf("�����������ʧ�� - index:%d - status:%d - name:%s - msgs:%s\n", index, status, name, msgs);
            return true;
        }
    }
}