// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <event_loop.h>
#include <event_loop_thread.h>
#include <event_share.h>
#include <tcp_socket.h>
#include <tcp_listen.h>
#include <tcp_server.h>

int main()
{
    printf("main thread:%d\n", GetCurrentThreadId());
    std::shared_ptr<evpp::EventShare> share = std::make_shared<evpp::EventShare>(2);
    share->CreaterLoops();
    evpp::EventLoop ev;
    ev.InitialEvent();

    evpp::TcpServer server(&ev, share);
    server.AddListenPort("127.0.0.1", 51322).AddListenPort("0.0.0.0", 51333).AddListenPort("::1", 51333);
    server.CreaterServer();
    ev.ExitDispatch();
    Sleep(100000);
    getchar();
}