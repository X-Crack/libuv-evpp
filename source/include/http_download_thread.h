#ifndef __HTTP_DOWNLOAD_THREAD_H__
#define __HTTP_DOWNLOAD_THREAD_H__
#include <event_config.h>
namespace Evpp
{
    class EventLoop;
    class EventLoopThreadEx;
    class HttpDownloadTask;
    class HttpDownloadThread
    {
    public:
        explicit HttpDownloadThread(EventLoop* base);
        virtual ~HttpDownloadThread();
    public:
    private:
        EventLoop*                                                      event_base;
        std::unique_ptr<EventLoopThreadEx>                              event_loop_thread_ex;
        std::unique_ptr<HttpDownloadTask>                               http_download_task;
    };
}
#endif // __HTTP_DOWNLOAD_THREAD_H__
