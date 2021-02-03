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
        return size * nmemb;
    }

    i32 HttpDownloadSession::OnProgress(double count_download_size, double current_download_size, double count_upload_size, double current_upload_size)
    {
        if (current_download_size > 0.0F)
        {
            EVENT_INFO("下载地址:%s 当前下载:%.2f%%", http_hosts.c_str(), ((current_download_size * 100.0F) / count_download_size));
            return 0;
        }
        return 0;
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

    i32 HttpDownloadSession::DefaultProgress(void* handler, double count_download_size, double current_download_size, double count_upload_size, double current_upload_size)
    {
        if (nullptr != handler)
        {
            HttpDownloadSession* watcher = static_cast<HttpDownloadSession*>(handler);
            {
                if (nullptr != watcher)
                {
                    return watcher->OnProgress(count_download_size, current_download_size, count_upload_size, current_upload_size);
                }
            }
        }
        return 0;
    }
}
