﻿#ifndef __HTTP_DOWNLOAD_POLL_H__
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
        explicit HttpDownloadPoll(EventLoop* loop, CURLM* curl_handler, const i32 fd, const CurlMessageTaskHandler& task_message);
        virtual ~HttpDownloadPoll();
    public:
        friend HttpDownloadMulti;
    public:
        void SetTaskMessageCallback(const CurlMessageTaskHandler& multi_task);
    private:
        void OnClose(HttpDownloadPoll* handler);
        void OnCurlPerform(event_poll* handler, int status, int events);
    private:
        static void DefaultClose(event_handle* handler);
        static void DefaultCurlPerform(event_poll* handler, int status, int events);
    private:
        void DownloadMessage(CURLMsg* message, int handles);
    private:
        EventLoop*                                                  event_base;
        event_poll*                                                 http_event_poll;
        CURLM*                                                      http_curl_handler;
        i32                                                         http_fd;
        CurlMessageTaskHandler                                      http_curl_task_message;
        i32                                                         http_curl_queue;
        i32                                                         http_curl_handles;
        String*                                                     http_curl_hosts;
    };
}
#endif // __HTTP_DOWNLOAD_POLL_H__
