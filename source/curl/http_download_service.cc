#include <http_download_service.h>
#include <http_download_session.h>
#include <http_download_poll.h>
#include <event_loop.h>

namespace Evpp
{
    HttpDownloadService::HttpDownloadService() : event_base(nullptr)
    {

    }

    HttpDownloadService::~HttpDownloadService()
    {

    }

    bool HttpDownloadService::CreaterDownload(const u96 index, EventLoop* loop, CURLM* curl_multi_handler, const String* host, const u32 port)
    {
        return CreaterDownload(index, loop, curl_easy_init(), curl_multi_handler, std::string(host), port);
    }

    bool HttpDownloadService::CreaterDownload(const u96 index, EventLoop* loop, CURLM* curl_multi_handler, const std::string& host, const u32 port)
    {
        return CreaterDownload(index, loop, curl_easy_init(), curl_multi_handler, host, port);
    }

    bool HttpDownloadService::CreaterDownload(const u96 index, EventLoop* loop, CURL* curl_easy_handler, CURLM* curl_multi_handler, const std::string& host, const u32 port)
    {
        if (CreaterDownloadSession(index, curl_easy_handler, host))
        {
            curl_easy_setopt(curl_easy_handler, CURLOPT_URL, host.c_str());
            if (port)
            {
                curl_easy_setopt(curl_easy_handler, CURLOPT_PORT, port);
            }
            curl_easy_setopt(curl_easy_handler, CURLOPT_SSL_VERIFYHOST, 0L);                                                        // 关闭SSL
            curl_easy_setopt(curl_easy_handler, CURLOPT_SSL_VERIFYPEER, 0L);                                                        // 关闭SSL

            curl_easy_setopt(curl_easy_handler, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl_easy_handler, CURLOPT_FOLLOWLOCATION, 1L);                                                        // 返回的头部中有Location(一般直接请求的url没找到)，则继续请求Location对应的数据
            curl_easy_setopt(curl_easy_handler, CURLOPT_CONNECTTIMEOUT, 120L);                                                      // 连接超时，这个数值如果设置太短可能导致数据请求不到就断开了
            curl_easy_setopt(curl_easy_handler, CURLOPT_TIMEOUT, 3000L);
            //curl_easy_setopt(handler, CURLOPT_NOBODY, 1L);                                                                        // 开启这个将下载失败 原因未知
            curl_easy_setopt(curl_easy_handler, CURLOPT_MAXREDIRS, 1L);

            curl_easy_setopt(curl_easy_handler, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.104 Safari/537.36");

            curl_easy_setopt(curl_easy_handler, CURLOPT_NOPROGRESS, 0L);                                                            // 设置进度响应
            curl_easy_setopt(curl_easy_handler, CURLOPT_PROGRESSDATA, GetDownloadSession(index));                                   // 数据传输的对象
            curl_easy_setopt(curl_easy_handler, CURLOPT_PROGRESSFUNCTION, &HttpDownloadSession::DefaultProgress);                   // 进度响应函数
            curl_easy_setopt(curl_easy_handler, CURLOPT_WRITEDATA, GetDownloadSession(index));
            curl_easy_setopt(curl_easy_handler, CURLOPT_WRITEFUNCTION, &HttpDownloadSession::DefaultMessage);

            return CURLMcode::CURLM_OK == curl_multi_add_handle(curl_multi_handler, curl_easy_handler);
        }
        return false;
    }

    bool HttpDownloadService::CreaterDownloadSession(const u96 index, CURL* handler, const std::string& host)
    {
        if (http_download_session.find(index) == http_download_session.end())
        {
            return http_download_session.emplace(index, std::make_shared<HttpDownloadSession>(handler, host)).second;
        }
        return false;
    }

    HttpDownloadSession* HttpDownloadService::GetDownloadSession(const u96 index)
    {
        return http_download_session[index].get();
    }
}
