#include <http_download_thread.h>
#include <event_loop.h>
#include <event_loop_thread_ex.h>
namespace Evpp
{
    HttpDownloadThread::HttpDownloadThread(EventLoop* base) :
        event_base(base),
        event_loop_thread_ex(std::make_unique<EventLoopThreadEx>(base, new event_loop(), 1)),
        http_download_task(std::make_unique<HttpDownloadTask>(event_loop_thread_ex->GetEventLoop()))
    {

    }

    HttpDownloadThread::~HttpDownloadThread()
    {

    }
}
