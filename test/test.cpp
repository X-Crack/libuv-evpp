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

#include <udp_listen.h>
#include <udp_server.h>

int main()
{
    printf("main thread:%d\n", GetCurrentThreadId());

    std::unique_ptr<Evpp::EventSocketPool> socket = std::make_unique<Evpp::EventSocketPool>();
    Evpp::EventLoop ev;
    ev.InitialEvent();
    Evpp::UdpServer udp(&ev);
    udp.AddListenPort("0.0.0.0", 60000);
    udp.AddListenPort("0.0.0.0", 60001);
    udp.CreaterServer();



    ev.ExecDispatch();
    getchar();

//     Evpp::EventLoop ev;
//     ev.InitialEvent();
// 
// 
//     Evpp::TcpClient client(&ev);
//     //client.AddListenPort("127.0.0.1", 60000);
//     client.AddListenPort("127.0.0.1", 60001);
//     client.SetConnectCallback();
//     client.SetRestoreCallback();
//     client.SetFailureCallback();
//     client.SetDisconsCallback();
//     client.SetMessageCallback();
//     client.CreaterClient();
//     client.SetReconnect(true);
//     ev.ExecDispatch();
//     getchar();



//    
//     std::shared_ptr<Evpp::EventShare> share = std::make_shared<Evpp::EventShare>(2);
//     share->CreaterLoops();
//     Evpp::EventLoop ev;
//     ev.InitialEvent();
// 
//     Evpp::TcpServer server(&ev, share);
//     server.AddListenPort("127.0.0.1", 51322).AddListenPort("0.0.0.0", 51333).AddListenPort("::1", 51333);
//     server.SetAcceptsCallback(std::bind(&Evpp::Import::DefaultAccepts, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
//     server.SetDisconsCallback(std::bind(&Evpp::Import::DefaultDiscons, std::placeholders::_1, std::placeholders::_2));
//     server.SetMessageCallback(std::bind(&Evpp::Import::DefaultMessage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
//     server.CreaterServer();
//     Evpp::EventTimerVesse timertest(&ev);
//     timertest.AssignTimer(1, 0, 3000);
// 
//     Evpp::EventSignal sn(&ev);
//     sn.InitialSignal();
//     sn.CreaterSignal();
//     ev.ExitDispatch();
//     printf("error exit\n");
//     getchar();
}