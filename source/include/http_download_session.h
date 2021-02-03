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
        explicit HttpDownloadSession(const std::string& hosts);
        virtual ~HttpDownloadSession();
    public:
        friend HttpDownloadMulti;
        friend HttpDownloadService;
    public:
        const std::string& GetDownloadHosts();
    private:
        u96 OnMessage(void* buffer, u96 size, u96 nmemb);
    private:
        static u96 DefaultMessage(void* buffer, u96 size, u96 nmemb, void* handler);
    private:
        std::string                                                 http_hosts;
    };
}
#endif // __HTTP_DOWNLOAD_SESSION_H__
