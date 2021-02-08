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

    bool HttpDownload::InitialCurlGlobal(const u96 thread_max_size, const long flags)
    {
        if (event_share->CreaterLoops(thread_max_size))
        {
            return CURLcode::CURLE_OK == curl_global_init(flags);
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

    void HttpDownload::SetMessageCallback(const u96 index, const CurlMessageHandler& message)
    {
        return GetDownloadTask(index)->SetMessageCallback(index, message);
    }

    void HttpDownload::SetProgressCallback(const u96 index, const CurlProgressHandler& progress)
    {
        return GetDownloadTask(index)->SetProgressCallback(index, progress);
    }

    void HttpDownload::SetTaskMessageCallback(const u96 index)
    {
        GetDownloadTask(index)->SetTaskMessageCallback(std::bind(&HttpDownload::OnDownloadMessage, this, std::placeholders::_1, std::placeholders::_2));
    }

    bool HttpDownload::RunInLoop(const Functor& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoop(function);
        }
        return false;
    }

    bool HttpDownload::RunInLoop(Functor&& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoop(std::move(function));
        }
        return false;
    }

    bool HttpDownload::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoopEx(function);
        }
        return false;
    }

    bool HttpDownload::RunInLoopEx(Handler&& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoopEx(std::move(function));
        }
        return false;
    }

    bool HttpDownload::InitialDownload(const u96 index)
    {
        std::unique_lock<std::mutex> lock(http_download_task_mutex);
        if (http_download_task.find(index) == http_download_task.end())
        {
            if (CreaterEventThread(index))
            {
                if (http_download_task.emplace(index, std::make_unique<HttpDownloadTask>(event_loop_thread_pool->GetEventLoop(index))).second)
                {
                    return true;
                }
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
                SetTaskMessageCallback(index);
                {
                    return CreaterDownload(index, GetDownloadTask(index), host, port);
                }
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
                SetTaskMessageCallback(index);
                {
                    return CreaterDownload(index, GetDownloadTask(index), host, port);
                }
            }
            return false;
        }
        return downloadtask->InitialDownload() && downloadtask->CreaterDownload(host, port);
    }

    HttpDownloadTask* HttpDownload::GetDownloadTask(const u96 index)
    {
        std::unique_lock<std::mutex> lock(http_download_task_mutex);
        if (http_download_task.find(index) != http_download_task.end())
        {
            return http_download_task[index].get();
        }
        return nullptr;
    }

    bool HttpDownload::CreaterEventThread(const u96 index)
    {
        return event_loop_thread_pool->CreaterEventThread(index) && event_loop_thread_pool->InitialEventThread(index);
    }

    bool HttpDownload::DestroyEventThread(const u96 index)
    {
        std::unique_lock<std::mutex> lock(http_download_task_mutex);
        if (http_download_task.find(index) != http_download_task.end())
        {
            http_download_task.erase(index);
            {
                if (event_loop_thread_pool->DestroyEventThread(index))
                {
                    return true;
                }
            }
        }

        return false;
    }

    void HttpDownload::OnDownloadMessage(const u96 index, const i32 http_curl_handles)
    {
        if (0 == http_curl_handles)
        {
            if (RunInLoopEx(std::bind(&HttpDownload::DestroyEventThread, this, index)))
            {
                EVENT_INFO("线程：%d 剩余任务：%d", index, http_curl_handles);
                EVENT_INFO("停止线程：%d", GetCurrentThreadId());
            }
        }
    }
}
