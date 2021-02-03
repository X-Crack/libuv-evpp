// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <event_evpp.h>
int main(int argc, char* argv[])
{
    std::shared_ptr<Evpp::EventShare> share = std::make_shared<Evpp::EventShare>();
    share->CreaterLoops(8);
    Evpp::EventLoop ev(share->DefaultEventLoop());
    ev.InitialEvent();

    Sleep(1000);
    Evpp::HttpDownload down(&ev);
    down.InitialCurlGlobal(16);
    down.CreaterDownload(1, "http://www.baidu.com", 80);
    //down.CreaterDownload(2, "https://mirrors.aliyun.com/centos/8.3.2011/isos/x86_64/CentOS-8.3.2011-x86_64-dvd1.iso", 443);
    //down.CreaterDownload(3, "https://mirrors.aliyun.com/centos/7.9.2009/isos/x86_64/CentOS-7-x86_64-Everything-2009.iso", 443);
    ev.ExecDispatch();
    printf("exit downloader\n");
    return 0;
    //     using namespace cpps; 
    //     C* c = cpps::create();
    //     _CPPS_TRY
    //         cpps::dofile(c, "./src.cpp");
    //     _CPPS_CATCH;
    //     cpps::close(c);
    getchar();
    return 0;
}
