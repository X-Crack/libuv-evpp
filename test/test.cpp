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

#include "cpps/cpps.h"



int main()
{
    using namespace cpps;
    C* c = cpps::create();
    _CPPS_TRY
     cpps::dofile(c, "./src.cpp");
    _CPPS_CATCH;
    //cpps::close(c);
    getchar();
}