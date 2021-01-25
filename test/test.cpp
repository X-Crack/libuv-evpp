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

#include <stdio.h>
#include <stdlib.h>

#define FIB_UNTIL 10
uv_loop_t* loop;

long fib_(long t)
{
    if (t == 0 || t == 1)
        return 1;
    else
        return fib_(t - 1) + fib_(t - 2);
}

void fib(uv_work_t* req)
{
    int n = *(int*)req->data;
    fprintf(stderr, "%dth fibonacci\n", n);
    int i = 0;
    if (rand() % 2)
    {
        i = 5;
        Sleep(5);
    }
    else
    {
        i = 10;
        Sleep(10);
    }

    long fib = fib_(n);
    fprintf(stderr, "%dth fibonacci is %lu---costTime %ds\n", n, fib, i);
}

void after_fib(uv_work_t* req, int status)
{
       fprintf(stderr, "Done calculating %dth fibonacci\n", *(int *) req->data);
}


int main()
{
    
    loop = uv_default_loop();

    int data[FIB_UNTIL];
    uv_work_t req[FIB_UNTIL];
    int i;
    for (i = 0; i < FIB_UNTIL; i++)
    {
        data[i] = i;
        req[i].data = (void*)&data[i];
        uv_queue_work(loop, &req[i], fib, after_fib);
    }

    return uv_run(loop, UV_RUN_DEFAULT);

    using namespace Evpp;
    TcpServerService tcp;
    tcp.AddListenPort("0.0.0.0", 6666);
//     tcp.AddListenPort("0.0.0.0", 6666);
//     tcp.AddListenPort("0.0.0.0", 7777);
    tcp.SetAcceptsCallback(Import::DefaultAccepts);
    tcp.SetDisconsCallback(Import::DefaultDiscons);
    tcp.SetMessageCallback(Import::DefaultMessage);

    //  bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index)
    //tcp.SetMessageCallback(std::bind(Import::DefaultMessage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    //uv_queue_work();

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