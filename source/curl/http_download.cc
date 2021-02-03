#include <http_download.h>
#include <http_download_multi.h>
#include <event_share.h>
#include <event_loop_thread_pool.h>
namespace Evpp
{
    HttpDownload::HttpDownload(EventLoop* loop) :
        event_base(loop),
        event_share(std::make_shared<EventShare>()),
        event_loop_thread_pool(std::make_unique<EventLoopThreadPool>(loop, event_share))
    {

    }

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
        if (InitialDownload(index))
        {
            return http_download_multi[index]->CreaterDownload(host, port);
        }
        return false;
    }

    bool HttpDownload::InitialDownload(const u96 index)
    {
        if (http_download_multi.find(index) == http_download_multi.end())
        {
            return http_download_multi.emplace(index, std::make_unique<HttpDownloadMulti>(event_loop_thread_pool->GetEventLoop(index))).second;
        }
        return true;
    }
}