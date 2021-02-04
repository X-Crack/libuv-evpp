#ifndef __HTTP_DOWNLOAD_SERVICE_H__
#define __HTTP_DOWNLOAD_SERVICE_H__
#include <event_config.h>
#include <unordered_map>
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
        bool CreaterDownload(const u96 index, EventLoop* loop, CURLM* curl_multi_handler, const String* host, const u32 port);
        bool CreaterDownload(const u96 index, EventLoop* loop, CURLM* curl_multi_handler, const std::string& host, const u32 port);
    public:
        void SetProxy(const std::string& proxy);
        void SetUserAgent(const std::string& agent);
    public:
        void SetMessageCallback(const u96 index, const CurlMessageHandler& message);
        void SetProgressCallback(const u96 index, const CurlProgressHandler& progress);
    private:
        bool CreaterDownload(const u96 index, EventLoop* loop, CURL* curl_easy_handler, CURLM* curl_multi_handler, const std::string& host, const u32 port);
    private:
        bool CreaterDownloadSession(const u96 index, CURL* handler, const std::string& host);
        bool DestroyDownloadSession(const u96 index);
        HttpDownloadSession* GetDownloadSession(const u96 index);
    private:
        EventLoop*                                                                      event_base;
        std::string                                                                     http_download_proxy;
        std::string                                                                     http_download_agent;
        std::unordered_map<u96, std::shared_ptr<HttpDownloadSession>>                   http_download_session;
    };
}
#endif // __HTTP_DOWNLOAD_SERVICE_H__
