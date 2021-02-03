#ifndef __HTTP_DOWNLOAD_MULTI_H__
#define __HTTP_DOWNLOAD_MULTI_H__
#include <event_config.h>
#include <memory>
namespace Evpp
{
    class EventShare;
    class EventLoop;
    class EventTimer;
    class EventLoopThreadPool;
    class HttpDownloadService;
    class HttpDownloadSession;
    class HttpDownloadPoll;
    class HttpDownloadMulti
    {
    public:
        explicit HttpDownloadMulti(EventLoop* loop);
        virtual ~HttpDownloadMulti();
    public:
        bool InitialDownload();
        bool CreaterDownload(const String* host, const u32 port = 80);
    private:
        void OnTaskTimer(EventLoop* loop, const std::shared_ptr<EventTimer>& timer, const u96 index);
    private:
        i32 OnSocket(CURL* easy, curl_socket_t fd, i32 action);
        i32 OnTimer(CURLM* multi, i32 delay);
    private:
        i32 InitialSocket(HttpDownloadPoll* poll, i32 action);
        i32 CreaterSocket(HttpDownloadSession* session, curl_socket_t fd, i32 action);
    private:
        static i32 DefaultSocket(CURL* easy, curl_socket_t fd, i32 action, void* handler, void* socketp);
        static i32 DefaultTimer(CURLM* multi, i32 timeout_ms, void* handler);
    private:
        EventLoop*                                                              event_base;
        std::shared_ptr<EventTimer>                                             event_timer;
        std::unique_ptr<HttpDownloadService>                                    http_download_service;
        CURLM*                                                                  http_curl_global_handler;
        i32                                                                     http_curl_global_handles;
    };
}
#endif // __HTTP_DOWNLOAD_MULTI_H__