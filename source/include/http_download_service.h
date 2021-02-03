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
        bool CreaterDownload(EventLoop* loop, CURLM* curl_multi_handler, const String* host, const u32 port);
        bool CreaterDownload(EventLoop* loop, CURLM* curl_multi_handler, const std::string& host, const u32 port);
    private:
        bool CreaterDownload(EventLoop* loop, CURL* curl_easy_handler, CURLM* curl_multi_handler, const std::string& host, const u32 port);
    private:
        bool CreaterDownloadSession(CURL* handler, const std::string& host);
        HttpDownloadSession* GetDownloadSession();
    private:
        EventLoop*                                                              event_base;
        std::shared_ptr<HttpDownloadSession>                                    http_download_session;
    };
}
#endif // __HTTP_DOWNLOAD_SERVICE_H__
