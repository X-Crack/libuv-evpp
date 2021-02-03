#ifndef __HTTP_DOWNLOAD_SERVICE_H__
#define __HTTP_DOWNLOAD_SERVICE_H__
#include <event_config.h>
namespace Evpp
{
    class EventLoop;
    class HttpDownloadMulti;
    class HttpDownloadSession;
    class HttpDownloadPoll;
    class HttpDownloadService
    {
    public:
        explicit HttpDownloadService();
        virtual ~HttpDownloadService();
    public:
        friend HttpDownloadMulti;
    public:
        bool CreaterDownload(EventLoop* loop, CURLM* curl_handler, const String* host, const u32 port);
    private:
        bool CreaterDownloadSession(const String* host);
        HttpDownloadSession* GetDownloadSession();
    private:
        bool CreaterDownloadPoll(EventLoop* loop, CURLM* curl_handler, const i32 fd);
        HttpDownloadPoll* GetDownloadPoll();
        bool DestroyPoll();
    private:
        EventLoop*                                                              event_base;
        std::shared_ptr<HttpDownloadSession>                                    http_download_session;
        std::shared_ptr<HttpDownloadPoll>                                       http_download_poll;
    };
}
#endif // __HTTP_DOWNLOAD_SERVICE_H__