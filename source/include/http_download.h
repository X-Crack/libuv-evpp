#ifndef __HTTP_DOWNLOAD_H__
#define __HTTP_DOWNLOAD_H__
#include <event_config.h>
#include <unordered_map>
namespace Evpp
{
    class EventShare;
    class EventLoop;
    class EventLoopThreadPool;
    class HttpDownloadMulti;
    class HttpDownload
    {
    public:
        explicit HttpDownload(EventLoop* loop);
        virtual ~HttpDownload();
    public:
        bool InitialCurlGlobal(const u96 thread_size, const long flags = CURL_GLOBAL_DEFAULT);
    public:
        bool CreaterDownload(const u96 index, const String* host, const u32 port = 80);
    private:
        bool InitialDownload(const u96 index);
    private:
        EventLoop* event_base;
        std::shared_ptr<EventShare>                                                     event_share;
        std::unique_ptr<EventLoopThreadPool>                                            event_loop_thread_pool;
        std::unordered_map<u96, std::unique_ptr<HttpDownloadMulti>>                     http_download_multi;
    };
}
#endif // __HTTP_DOWNLOAD_H__
