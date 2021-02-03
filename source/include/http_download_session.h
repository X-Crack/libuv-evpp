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
        explicit HttpDownloadSession(CURL* easy_cyrl, const std::string& hosts);
        virtual ~HttpDownloadSession();
    public:
        friend HttpDownloadMulti;
        friend HttpDownloadService;
    public:
        const std::string& GetDownloadHosts();
    private:
        u96 OnMessage(void* buffer, u96 size, u96 nmemb);
        i32 OnProgress(double count_download_size, double current_download_size, double count_upload_size, double current_upload_size);
    private:
        static u96 DefaultMessage(void* buffer, u96 size, u96 nmemb, void* handler);
        static i32 DefaultProgress(void* handler, double count_download_size, double current_download_size, double count_upload_size, double current_upload_size);
    private:
        CURL*                                                       http_easy_curl;
        std::string                                                 http_hosts;
        String*                                                     http_curl_hosts;
        double                                                      original_download_size;
    };
}
#endif // __HTTP_DOWNLOAD_SESSION_H__
