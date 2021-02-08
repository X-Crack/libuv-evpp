#include "echo_server.h"
#include <event_share.h>
#include <event_loop.h>
#include <tcp_server.h>
#include <tcp_session.h>
#include <event_buffer.h>
namespace Evpp
{

    /*
    * u32 = uint32_t
    * u64 = uint64_t
    * u96 = size_t
    */

    EchoServer::EchoServer()
    {
        event_share.reset(new EventShare());
        event_base.reset(new EventLoop(event_share->DefaultEventLoop()));
        tcp_server.reset(new TcpServer(event_base.get(), event_share));
    }

    EchoServer::~EchoServer()
    {

    }

    void EchoServer::RunExamples()
    {
        event_share->CreaterLoops(EventShare::GetHardwareThreads());

        event_base->InitialEvent();

        // listen ipv4
        tcp_server->AddListenPort("0.0.0.0", 5555);
        tcp_server->AddListenPort("0.0.0.0", 6666);
        tcp_server->AddListenPort("0.0.0.0", 7777);
        tcp_server->AddListenPort("0.0.0.0", 8888);
        // listen ipv6
        tcp_server->AddListenPort("::1", 5555);
        tcp_server->AddListenPort("::1", 6666);
        tcp_server->AddListenPort("::1", 7777);
        tcp_server->AddListenPort("::1", 8888);

        tcp_server->SetAcceptsCallback(std::bind(&EchoServer::OnAccepts, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        tcp_server->SetDisconsCallback(std::bind(&EchoServer::OnDiscons, this, std::placeholders::_1, std::placeholders::_2));
        tcp_server->SetMessageCallback(std::bind(&EchoServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        tcp_server->SetSendMsgCallback(std::bind(&EchoServer::OnSendMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

        tcp_server->CreaterServer(event_share->GetLoopsSize());

        event_base->ExecDispatch();

    }

    bool EchoServer::OnAccepts(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index)
    {
        std::cout << "进入离开: " << index << std::endl;
        return true;
    }

    bool EchoServer::OnDiscons(EventLoop* loop, const u96 index)
    {
        std::cout << "用户离开: " << index << std::endl;
        return true;
    }

    bool EchoServer::OnMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<EventBuffer>& buffer, const u96 index)
    {
        std::cout << "用户消息: " << index << " " << "消息长度: %d" << buffer->readableBytes() << std::endl;
        buffer->retrieve(buffer->readableBytes());
        return true;
    }

    bool EchoServer::OnSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status)
    {
        std::cout << "发送消息: " << index << " " << "发送状态: %d" << status << std::endl;
        return true;
    }
}
