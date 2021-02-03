#include <http_download.h>
#include <http_download_multi.h>
#include <event_share.h>
#include <event_loop.h>
#include <event_loop_thread_pool.h>
namespace Evpp
{
    HttpDownload::HttpDownload(EventLoop* loop) :
        event_base(loop),
        event_share(std::make_shared<EventShare>()),
        event_loop_thread_pool(std::make_unique<EventLoopThreadPool>(loop, event_share))
    {

    }
    // 断点下载 https://www.cnblogs.com/chang290/archive/2012/08/12/2634858.html
    HttpDownload::~HttpDownload()
    {
        curl_global_cleanup();
    }

    bool HttpDownload::InitialCurlGlobal(const u96 thread_size, const long flags)
    {
        if (event_share->CreaterLoops(thread_size))
        {
            if (event_loop_thread_pool->CreaterEventThreadPool(thread_size))
            {
                return CURLcode::CURLE_OK == curl_global_init(flags);
            }
        }
        return false;
    }

    bool HttpDownload::CreaterDownload(const u96 index, const String* host, const u32 port)
    {
        return CreaterDownload(index, std::string(host), port);
    }

    bool HttpDownload::CreaterDownload(const u96 index, const std::string& host, const u32 port)
    {
        return InitialDownload(event_loop_thread_pool->GetEventLoop(index), index, host, port);
    }

    bool HttpDownload::InitialDownload(EventLoop* loop, const u96 index, const std::string& host, const u32 port)
    {
        if (http_download_multi.find(index) == http_download_multi.end())
        {
            if (http_download_multi.emplace(index, std::make_unique<HttpDownloadMulti>(loop)).second)
            {
                return CreaterDownloadEx(loop, index, host, port);
            }
        }
        return true;
    }

    bool HttpDownload::CreaterDownloadEx(EventLoop* loop, const u96 index, const std::string& host, const u32 port)
    {
        if (loop->EventThread())
        {
            return http_download_multi[index]->InitialDownload() && http_download_multi[index]->CreaterDownload(host, port);
        }
        return loop->RunInLoopEx(std::bind(&HttpDownload::CreaterDownloadEx, this, loop, index, host, port));
    }
}
