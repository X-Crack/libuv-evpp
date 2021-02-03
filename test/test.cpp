// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <event_evpp.h>
#include <http_download_multi.h>
#include <http_download.h>

int main(int argc, char* argv[])
{

    Evpp::EventLoop ev;
    ev.InitialEvent();

    Evpp::HttpDownload dow(&ev);
    dow.InitialCurlGlobal(8);
    dow.CreaterDownload(1, "https://github.com/v2ray/v2ray-core/releases/download/v4.28.2/v2ray-linux-mips32.zip", 443);
    dow.CreaterDownload(2, "https://mirrors.aliyun.com/centos/8.3.2011/isos/x86_64/CentOS-8.3.2011-x86_64-dvd1.iso", 443);
    //     Evpp::HttpDownloadMulti down(&ev);
    //     down.CreaterDownload("https://github.com/v2ray/v2ray-core/releases/download/v4.28.2/v2ray-linux-mips32.zip");
    ev.ExecDispatch();
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
