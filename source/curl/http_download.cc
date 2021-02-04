#include <http_download.h>
#include <http_download_multi.h>
#include <http_download_task.h>
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
        return CreaterDownload(index, GetDownloadTask(index), host, port);
    }

    bool HttpDownload::CreaterDownload(const u96 index, const std::string& host, const u32 port)
    {
        return CreaterDownload(index, GetDownloadTask(index), host, port);
    }

    bool HttpDownload::InitialDownload(const u96 index)
    {
        if (http_download_task.find(index) == http_download_task.end())
        {
            if (http_download_task.emplace(index, std::make_unique<HttpDownloadTask>(event_loop_thread_pool->GetEventLoop(index))).second)
            {
                return true;
            }
        }
        return true;
    }

    bool HttpDownload::CreaterDownload(const u96 index, HttpDownloadTask* downloadtask, const String* host, const u32 port)
    {
        if (nullptr == downloadtask)
        {
            if (InitialDownload(index))
            {
                return CreaterDownload(index, GetDownloadTask(index), host, port);
            }
            return false;
        }
        return downloadtask->InitialDownload() && downloadtask->CreaterDownload(host, port);
    }

    bool HttpDownload::CreaterDownload(const u96 index, HttpDownloadTask* downloadtask, const std::string& host, const u32 port)
    {
        if (nullptr == downloadtask)
        {
            if (InitialDownload(index))
            {
                return CreaterDownload(index, GetDownloadTask(index), host, port);
            }
            return false;
        }
        return downloadtask->InitialDownload() && downloadtask->CreaterDownload(host, port);
    }

    HttpDownloadTask* HttpDownload::GetDownloadTask(const u96 index)
    {
        if (http_download_task.find(index) != http_download_task.end())
        {
            return http_download_task[index].get();
        }
        return nullptr;
    }
}
