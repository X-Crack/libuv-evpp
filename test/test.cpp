// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
// #include <mimalloc.h>
// #include <mimalloc-new-delete.h>
#include <event_evpp.h>
#include <event_queue.h>

void stop_server(Evpp::TcpServerService* server)
{
    if (server->DestroyServer())
    {
       // EVENT_INFO("OK\n");
        return;
    }
    assert(0);
}

void stop_client(Evpp::TcpClientService* client)
{
    Sleep(5000);
    if (client->DestroyClient())
    {
        // EVENT_INFO("OK\n");
        return;
    }
    assert(0);
}

void printf_ex(Evpp::EventLoop* loop)
{
    //EVENT_INFO("%d", loop->GetEventIndex());
}

int main(int argc, char* argv[])
{
    //mi_version();
    //mi_stats_reset();

    using namespace Evpp; 

//     {
//         EVENT_COMPUTE_DURATION(全程耗时);
//         std::unique_ptr<TcpClientService> client = std::make_unique<TcpClientService>();
//         client->AddServerPort("127.0.0.1", 5555);
//         client->SetConnectCallback();
//         client->SetDisconsCallback();
//         client->SetFailureCallback();
//         client->SetMessageCallback();
//         client->SetRestoreCallback();
//         client->SetSendMsgCallback();
//         client->CreaterClient();
//         std::thread T1(std::bind(&stop_client, client.get()));
//         client->ExecDispatchCoroutine(printf_ex);
//         T1.join();
//         client.reset();
//     }
//     EventLoop ev;
//     EventQueue queue(&ev);
//     ev.InitialEvent();
//     queue.CreaterQueue();
//     queue.RunInQueueEx(std::bind(printf_ex, &ev));
//     //std::thread T1(std::bind(&asdjkajsdjasd, &ev, &queue));
//     //T1.detach();
//     ev.ExecDispatch();
     //while (true)
     {
         {
             EVENT_COMPUTE_DURATION(全程耗时);
             std::unique_ptr<TcpServerService> server = std::make_unique<TcpServerService>();
             server->AddListenPort("0.0.0.0", 5555);
             server->AddListenPort("0.0.0.0", 6666);
             server->AddListenPort("0.0.0.0", 7777);
             server->AddListenPort("::", 5555);
             server->AddListenPort("::", 6666);
             server->AddListenPort("::", 7777);
             server->SetAcceptsCallback();
             server->SetDisconsCallback();
             server->SetMessageCallback();
             server->SetSendMsgCallback();
             if (0 == server->CreaterServer(EventShare::GetHardwareThreads()))
             {
                 printf("创建服务器失败\n");
                 Sleep(999999);
             }
             //std::thread T1(std::bind(&stop_server, server.get()));
             server->ExecDispatchCoroutine(printf_ex);
             //T1.join();
             //EVENT_INFO("exit\n");
             server.reset();
         }
     }

    getchar();
    //     EventLoop ev;
    //     ev.InitialEvent();
    //     Evpp::HttpDownload dow(&ev);
    //     dow.InitialCurlGlobal(16);
    //     dow.CreaterDownload(0, "https://www.163.com");
    //     dow.CreaterDownload(0, "https://www.baidu.com");
    //     dow.CreaterDownload(0, "https://www.baidu.com");
    //     dow.CreaterDownload(1, "https://www.163.com");
    //     dow.CreaterDownload(1, "https://www.qq.com");
    //     dow.CreaterDownload(1, "https://www.qq.com");
    //     dow.CreaterDownload(1, "https://www.qq.com");
    //     dow.CreaterDownload(1, "https://www.qq.com");
    //     dow.CreaterDownload(1, "https://www.qq.com");
    //     dow.CreaterDownload(1, "https://www.qq.com");
    //     dow.CreaterDownload(2, "https://www.163.com");
    //     dow.CreaterDownload(2, "https://www.163.com");
    //     dow.CreaterDownload(3, "https://www.126.com");
    //     dow.CreaterDownload(4, "https://www.baidu.com");
    //     dow.CreaterDownload(5, "https://www.qq.com");
    //     dow.CreaterDownload(6, "https://www.163.com");
    //     dow.CreaterDownload(7, "https://www.126.com");
    //     dow.CreaterDownload(8, "https://www.baidu.com");
    //     dow.CreaterDownload(9, "https://www.qq.com");
    //     dow.CreaterDownload(11, "https://www.126.com");
    //     ev.ExecDispatch();
        //dow.CreaterDownload(1, "https://www.88.com");
        //dow.CreaterDownload(1, "https://mirrors.aliyun.com/centos/8.3.2011/isos/x86_64/CentOS-8.3.2011-x86_64-dvd1.iso", 443);
        //     Evpp::HttpDownloadMulti down(&ev);
        //     down.CreaterDownload("https://github.com/v2ray/v2ray-core/releases/download/v4.28.2/v2ray-linux-mips32.zip");

    return 0;
    //     using namespace cpps; 
    //     C* c = cpps::create();
    //     _CPPS_TRY
    //         cpps::dofile(c, "./src.cpp");
    //     _CPPS_CATCH;
    //     cpps::close(c);
    getchar();
    //mi_collect(true);
    return 0;
}
