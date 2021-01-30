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
#include <event_timer_pool.h>

#include <event_signal.h>
#include <event_loop.h>
#include <tcp_client.h>
#include <tcp_server_service.h>
#include <event_work_queue.h>
#include <event_resolve.h>
#include <tcp_client_service.h>
//#include "cpps/cpps.h"

#include <future>

void exit_loop(Evpp::TcpClientService* client)
{
    Sleep(5000);
    u32 timer = GetTickCount();
    client->DestroyClient(true);
    printf("Exit OK %d\n", GetTickCount() - timer);
}

using namespace Evpp;
int main()
{

    TcpClientService client;

    client.AddListenPort("127.0.0.1", 8888);

    client.SetConnectCallback();
    client.SetDisconsCallback();
    client.SetFailureCallback();
    client.SetMessageCallback();
    client.SetRestoreCallback();
    client.SetSendMsgCallback();

    client.CreaterClient();

    //std::thread t1(std::bind(exit_loop, &client));
    //t1.detach();

    client.ExecDispatch();


    return 0;
    TcpServerService Tcp;

    Tcp.AddListenPort("0.0.0.0", 8888);
    Tcp.AddListenPort("0.0.0.0", 7777);
    Tcp.AddListenPort("0.0.0.0", 9999);
    Tcp.SetAcceptsCallback(Import::DefaultAccepts);
    Tcp.SetDisconsCallback(Import::DefaultDiscons);
    Tcp.SetMessageCallback(Import::DefaultMessage);
    Tcp.SetSendMsgCallback(Import::DefaultSendMsg);
    //Tcp.SetEventThreadId(GetCurrentThreadId());
    Tcp.CreaterServer(16);
    Tcp.ExecDispatch();
    printf("exit thread\n");

    //printf("异常退出\n");
//     using namespace cpps; 
//     C* c = cpps::create();
//     _CPPS_TRY
//         cpps::dofile(c, "./src.cpp");
//     _CPPS_CATCH;
//     cpps::close(c);
    getchar();
}