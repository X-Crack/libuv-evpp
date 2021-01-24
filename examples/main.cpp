#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "echo_server/echo_server.h"
int main()
{
    using namespace Evpp;
    EchoServer echo_server;
    echo_server.RunExamples();
    return 0;
}