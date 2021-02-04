#ifndef __HTTP_DOWNLOAD_H__
#define __HTTP_DOWNLOAD_H__
#include <event_config.h>
#include <unordered_map>
namespace Evpp
{
    class EventShare;
    class EventLoop;
    class EventLoopThreadPool;
    class HttpDownloadTask;
    class HttpDownload
    {
    public:
        explicit HttpDownload(EventLoop* loop);
        virtual ~HttpDownload();
    public:
        bool InitialCurlGlobal(const u96 thread_size, const long flags = CURL_GLOBAL_DEFAULT);
    public:
        bool CreaterDownload(const u96 index, const String* host, const u32 port = 0);
        bool CreaterDownload(const u96 index, const std::string& host, const u32 port = 0);
    public:
        void SetMessageCallback(const u96 index, const CurlMessageHandler& message);
        void SetProgressCallback(const u96 index, const CurlProgressHandler& progress);
        void SetTaskMessageCallback(const u96 index);
    public:
        bool RunInLoop(const Functor& function);
        bool RunInLoop(Functor&& function);
        bool RunInLoopEx(const Handler& function);
        bool RunInLoopEx(Handler&& function);
    private:
        bool InitialDownload(const u96 index);
    private:
        bool CreaterDownload(const u96 index, HttpDownloadTask* downloadtask, const String* host, const u32 port);
        bool CreaterDownload(const u96 index, HttpDownloadTask* downloadtask, const std::string& host, const u32 port);
        HttpDownloadTask* GetDownloadTask(const u96 index);
    private:
        bool CreaterEventThread(const u96 index, const bool use_thread_ex);
        bool DestroyEventThread(const u96 index, const bool use_thread_ex);
    private:
        void OnDownloadMessage(const u96 index, const i32 http_curl_handles);
    private:
        EventLoop*                                                                      event_base;
        std::shared_ptr<EventShare>                                                     event_share;
        std::unique_ptr<EventLoopThreadPool>                                            event_loop_thread_pool;
        std::unordered_map<u96, std::unique_ptr<HttpDownloadTask>>                      http_download_task;
    };
}
#endif // __HTTP_DOWNLOAD_H__
