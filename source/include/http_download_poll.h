#ifndef __HTTP_DOWNLOAD_POLL_H__
#define __HTTP_DOWNLOAD_POLL_H__
#include <event_config.h>
namespace Evpp
{
    class EventLoop;
    class EventTimer;
    class HttpDownloadMulti;
    class HttpDownloadPoll
    {
    public:
        explicit HttpDownloadPoll(EventLoop* loop, CURLM* curl_handler, const i32 fd);
        virtual ~HttpDownloadPoll();
    public:
        friend HttpDownloadMulti;
    private:
        void OnClose(HttpDownloadPoll* handler);
        void OnCurlPerform(uv_poll_t* handler, int status, int events);
    private:
        static void DefaultClose(uv_handle_t* handler);
        static void DefaultCurlPerform(uv_poll_t* handler, int status, int events);
    private:
        void DownloadMessage(CURLMsg* message);
    private:
        EventLoop*                                                  event_base;
        event_poll*                                                 http_event_poll;
        CURLM*                                                      http_curl_handler;
        i32                                                         http_fd;
        i32                                                         http_curl_queue;
        i32                                                         http_curl_handles;
        String*                                                     http_curl_hosts;
    };
}
#endif // __HTTP_DOWNLOAD_POLL_H__
