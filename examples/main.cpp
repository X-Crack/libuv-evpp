#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "echo_server/echo_server.h"
#include "stop_loop/stop_loop.h"
int main()
{
    using namespace Evpp;
//     EchoServer echo_server;
//     echo_server.RunExamples();

    StopLoop stop_loop;
    stop_loop.RunExamples3();
    return 0;
}