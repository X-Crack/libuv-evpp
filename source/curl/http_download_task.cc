#include <http_download_task.h>
#include <http_download_multi.h>
#include <event_loop.h>
#include <event_loop_thread_ex.h>
namespace Evpp
{
    HttpDownloadTask::HttpDownloadTask(EventLoop* base) :
        event_base(base),
        http_download_multi(std::make_unique<HttpDownloadMulti>(base))
    {

    }

    HttpDownloadTask::~HttpDownloadTask()
    {

    }

    bool HttpDownloadTask::InitialDownload()
    {
        if (nullptr != http_download_multi)
        {
            if (ExistsRuning())
            {
                return true;
            }

            if (event_base->EventThread())
            {
                if (http_download_multi->InitialDownload(curl_multi_init()))
                {
                    return ChangeStatus(Status::Exec);
                }
            }
            return RunInLoop(std::bind(&HttpDownloadTask::InitialDownload, this));
        }
        return false;
    }

    bool HttpDownloadTask::CreaterDownload(const String* host, const u32 port)
    {
        if (nullptr != http_download_multi)
        {
            if (event_base->EventThread())
            {
                return http_download_multi->CreaterDownload(http_download_task.fetch_add(1), host, port);
            }
            return RunInLoop(std::bind((bool(HttpDownloadTask::*)(const String *, const u32)) & HttpDownloadTask::CreaterDownload, this, host, port));
        }
        return false;
    }

    bool HttpDownloadTask::CreaterDownload(const std::string& host, const u32 port)
    {
        if (nullptr != http_download_multi)
        {
            if (event_base->EventThread())
            {
                return http_download_multi->CreaterDownload(http_download_task.fetch_add(1), host, port);
            }
            return RunInLoopEx(std::bind((bool(HttpDownloadTask::*)(const std::string&, const u32)) & HttpDownloadTask::CreaterDownload, this, host, port));
        }
        return false;
    }

    void HttpDownloadTask::SetMessageCallback(const u96 index, const CurlMessageHandler& message)
    {
        return http_download_multi->SetMessageCallback(index, message);
    }

    void HttpDownloadTask::SetProgressCallback(const u96 index, const CurlProgressHandler& progress)
    {
        return http_download_multi->SetProgressCallback(index, progress);
    }

    void HttpDownloadTask::SetTaskMessageCallback(const CurlMessageTaskHandler& task_message)
    {
        return http_download_multi->SetTaskMessageCallback(task_message);
    }

    bool HttpDownloadTask::RunInLoop(const Functor& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoop(function);
        }
        return false;
    }

    bool HttpDownloadTask::RunInLoop(Functor&& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoop(std::move(function));
        }
        return false;
    }

    bool HttpDownloadTask::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoopEx(function);
        }
        return false;
    }

    bool HttpDownloadTask::RunInLoopEx(Handler&& function)
    {
        if (nullptr != event_base)
        {
            return event_base->RunInLoopEx(std::move(function));
        }
        return false;
    }
}
