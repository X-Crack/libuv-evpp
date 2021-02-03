#include <http_download_session.h>
namespace Evpp
{
    HttpDownloadSession::HttpDownloadSession(const std::string& hosts) :
        http_hosts(hosts)
    {

    }

    HttpDownloadSession::~HttpDownloadSession()
    {

    }

    const std::string& HttpDownloadSession::GetDownloadHosts()
    {
        return http_hosts;
    }

    u96 HttpDownloadSession::OnMessage(void* buffer, u96 size, u96 nmemb)
    {
        EVENT_INFO("下载地址:%s 当前下载:%d", http_hosts.c_str(), size * nmemb);
        return size * nmemb;
    }

    u96 HttpDownloadSession::DefaultMessage(void* buffer, u96 size, u96 nmemb, void* handler)
    {
        if (nullptr != handler)
        {
            HttpDownloadSession* watcher = static_cast<HttpDownloadSession*>(handler);
            {
                if (nullptr != watcher)
                {
                    return watcher->OnMessage(buffer, size, nmemb);
                }
            }
        }
        return 0;
    }
}
