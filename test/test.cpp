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
//#include "cpps/cpps.h"

#include <future>
using namespace Evpp;

void exitserver(TcpServerService* Tcp)
{
    Sleep(13000);
    Tcp->DestroyServer();
    printf("okok\n");
    while (1)
    {
        Sleep(1000);
    }
}


void Resolvecallback(EventLoop* loop, const std::vector<std::string>& address_list)
{
    printf("\n");
}

void ExecResolvecallback(EventLoop* loop)
{
    printf("\n");

    Sleep(3000);

    EventResolve rl(loop);


    rl.SetGetaddrInfoExCallback(Resolvecallback);
    rl.GetAddressInfo("www.baidu.com", "");

    Sleep(9999999);
}


void Run()
{


//     EventLoop ev;
// 
// 
//     ev.InitialEvent();
//     std::unique_ptr<std::thread> thread1 = std::make_unique<std::thread>(std::bind(ExecResolvecallback, &ev));
//     ev.ExecDispatch();
// 
//     return;
    TcpServerService Tcp;

    Tcp.AddListenPort("0.0.0.0", 8888);
    Tcp.AddListenPort("0.0.0.0", 7777);
    Tcp.AddListenPort("0.0.0.0", 9999);
    Tcp.SetAcceptsCallback(Import::DefaultAccepts);
    Tcp.SetDisconsCallback(Import::DefaultDiscons);
    Tcp.SetMessageCallback(Import::DefaultMessage);
    //Tcp.SetEventThreadId(GetCurrentThreadId());
    Tcp.CreaterServer(16);
    std::unique_ptr<std::thread> thread = std::make_unique<std::thread>(std::bind(exitserver, &Tcp));
    thread->detach();
    Tcp.ExecDispatch();
    printf("exit thread\n");
}

int main()
{
    
    
//     EventLoop ev;
//     EventWorkQueue qe(&ev);
//     qe.SetCreaterCallback(crcallback);
//     qe.AssignWorkQueue();
//     
//     ev.ExecDispatch();
//    return 0;

    TcpServerService Tcp;

    Tcp.AddListenPort("0.0.0.0", 8888);
    Tcp.AddListenPort("0.0.0.0", 7777);
    Tcp.AddListenPort("0.0.0.0", 9999);
    Tcp.SetAcceptsCallback(Import::DefaultAccepts);
    Tcp.SetDisconsCallback(Import::DefaultDiscons);
    Tcp.SetMessageCallback(Import::DefaultMessage);
    //Tcp.SetEventThreadId(GetCurrentThreadId());
    Tcp.CreaterServer(16);
    std::unique_ptr<std::thread> thread = std::make_unique<std::thread>(std::bind(exitserver, &Tcp));
    thread->detach();
    Tcp.ExecDispatch();
    return 0;

//     Sleep(10000);
//     if (Tcp.DestroyServer())
//     {
//         printf("服务器停止成功\n");
//     }
//     else
//     {
//         printf("服务器停止失败\n");
//     }
    //printf("异常退出\n");
//     using namespace cpps; 
//     C* c = cpps::create();
//     _CPPS_TRY
//         cpps::dofile(c, "./src.cpp");
//     _CPPS_CATCH;
//     cpps::close(c);
    getchar();
}