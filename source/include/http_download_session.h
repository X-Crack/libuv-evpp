#ifndef __HTTP_DOWNLOAD_SESSION_H__
#define __HTTP_DOWNLOAD_SESSION_H__
#include <event_config.h>
namespace Evpp
{
    class HttpDownloadMulti;
    class HttpDownloadService;
    class HttpDownloadSession
    {
    public:
        explicit HttpDownloadSession(const u96 index, CURL* easy_cyrl, const std::string& hosts, const CurlMessageHandler& message, const CurlProgressHandler& progress);
        virtual ~HttpDownloadSession();
    public:
        friend HttpDownloadMulti;
        friend HttpDownloadService;
    public:
        void SetMessageCallback(const CurlMessageHandler& message);
        void SetProgressCallback(const CurlProgressHandler& progress);
    public:
        const std::string& GetDownloadHosts();
    private:
        u96 OnMessage(void* buffer, u96 size, u96 nmemb);
        i32 OnProgress(double count_download_size, double current_download_size, double count_upload_size, double current_upload_size);
    private:
        static u96 DefaultMessage(void* buffer, u96 size, u96 nmemb, void* handler);
        static i32 DefaultProgress(void* handler, double count_download_size, double current_download_size, double count_upload_size, double current_upload_size);
    private:
        u96                                                         http_index;
        CURL*                                                       http_easy_curl;
        std::string                                                 http_hosts;
        String*                                                     http_curl_hosts;
        u32                                                         http_curl_response_code;
        CurlMessageHandler                                          http_curl_message;
        CurlProgressHandler                                         http_curl_progress;
        double                                                      original_download_size;
    };
}
#endif // __HTTP_DOWNLOAD_SESSION_H__
