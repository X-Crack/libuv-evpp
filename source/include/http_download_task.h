#ifndef __HTTP_DOWNLOAD_TASK_H__
#define __HTTP_DOWNLOAD_TASK_H__
#include <event_config.h>
#include <event_status.h>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class EventStatus;
    class HttpDownloadMulti;
    class HttpDownloadTask : public EventStatus
    {
    public:
        explicit HttpDownloadTask(EventLoop* loop);
        virtual ~HttpDownloadTask();
    public:
        bool InitialDownload();
        bool CreaterDownload(const String* host, const u32 port);
        bool CreaterDownload(const std::string& host, const u32 port);
    private:
        bool RunInLoop(const Functor& function);
        bool RunInLoop(Functor&& function);
        bool RunInLoopEx(const Handler& function);
        bool RunInLoopEx(Handler&& function);
    private:
        EventLoop*                                                                              event_base;
        std::atomic<u96>                                                                        http_download_task;
        std::unique_ptr<HttpDownloadMulti>                                                      http_download_multi;
    };
}
#endif // __HTTP_DOWNLOAD_TASK_H__
