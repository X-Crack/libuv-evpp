// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <event_loop.h>
#include <event_loop_thread.h>
#include <event_share.h>
#include <event_socket.h>
#include <event_socket_pool.h>

#include <tcp_listen.h>
#include <tcp_server.h>

#include <event_timer.h>
#include <event_timer_vesse.h>

#include <event_signal.h>

#include <tcp_client.h>
#include <tcp_server_service.h>
//#include "cpps/cpps.h"



int main()
{
    using namespace Evpp;
    TcpServerService tcp;
    tcp.AddListenPort("0.0.0.0", 5555);
    tcp.AddListenPort("0.0.0.0", 6666);
    tcp.AddListenPort("0.0.0.0", 7777);
    tcp.SetAcceptsCallback(Import::DefaultAccepts);
    tcp.SetDisconsCallback(Import::DefaultDiscons);
    tcp.SetMessageCallback(Import::DefaultMessage);


    //  bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index)
    //tcp.SetMessageCallback(std::bind(Import::DefaultMessage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));


    tcp.CreaterServer(16);
    tcp.ExecDispatch();
    printf("异常退出\n");
//     using namespace cpps;
//     C* c = cpps::create();
//     _CPPS_TRY
//         cpps::dofile(c, "./src.cpp");
//     _CPPS_CATCH;
//     cpps::close(c);
    getchar();
}