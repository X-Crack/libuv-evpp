#include <http_download_service.h>
#include <http_download_session.h>
#include <http_download_poll.h>
#include <event_loop.h>

namespace Evpp
{
    HttpDownloadService::HttpDownloadService() :
        event_base(nullptr),
        http_download_proxy(/*"127.0.0.1:64882"*/),
        http_download_agent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.104 Safari/537.36")
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

    void HttpDownloadService::SetProxy(const std::string& proxy)
    {
        http_download_proxy = proxy;
    }

    void HttpDownloadService::SetUserAgent(const std::string& agent)
    {
        http_download_agent = agent;
    }

    void HttpDownloadService::SetMessageCallback(const u96 index, const CurlMessageHandler& message)
    {
        if (nullptr == http_curl_message && nullptr != message)
        {
            http_curl_message = message;
        }
    }

    void HttpDownloadService::SetProgressCallback(const u96 index, const CurlProgressHandler& progress)
    {
        if (nullptr == http_curl_progress && nullptr != progress)
        {
            http_curl_progress = progress;
        }
    }

    bool HttpDownloadService::CreaterDownload(const u96 index, EventLoop* loop, CURL* curl_easy_handler, CURLM* curl_multi_handler, const std::string& host, const u32 port)
    {
        if (CreaterDownloadSession(index, curl_easy_handler, host))
        {
            if (CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_URL, host.c_str()) &&
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_PORT, port) &&
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_SSL_VERIFYHOST, 0L) &&                                            // 关闭SSL
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_SSL_VERIFYPEER, 0L) &&                                            // 关闭SSL

                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_VERBOSE, 1L) &&
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_FOLLOWLOCATION, 1L) &&                                            // 返回的头部中有Location(一般直接请求的url没找到)，则继续请求Location对应的数据
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_CONNECTTIMEOUT, 30L) &&                                           // 连接超时，这个数值如果设置太短可能导致数据请求不到就断开了
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_TIMEOUT, 3000L) &&
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_MAXREDIRS, 5L) &&
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_PROXY, http_download_proxy.c_str()) &&
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_USERAGENT, http_download_agent.c_str()) &&
                //CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_RESUME_FROM_LARGE, 0) &&                                          //断点下载设置

                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_NOPROGRESS, 0L) &&                                                // 设置进度响应
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_XFERINFODATA, GetDownloadSession(index)) &&                       // 进度响应指针
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_PROGRESSFUNCTION, &HttpDownloadSession::DefaultProgress) &&       // 进度响应函数
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_WRITEDATA, GetDownloadSession(index)) &&                          // 数据响应指针
                CURLcode::CURLE_OK == curl_easy_setopt(curl_easy_handler, CURLOPT_WRITEFUNCTION, &HttpDownloadSession::DefaultMessage))             // 数据响应回调
            {
                return CURLMcode::CURLM_OK == curl_multi_add_handle(curl_multi_handler, curl_easy_handler);
            }
        }
        return false;
    }

    bool HttpDownloadService::CreaterDownloadSession(const u96 index, CURL* handler, const std::string& host)
    {
        if (http_download_session.find(index) == http_download_session.end())
        {
            return http_download_session.emplace(index, std::make_shared<HttpDownloadSession>(index, handler, host, http_curl_message, http_curl_progress)).second;
        }
        return false;
    }

    bool HttpDownloadService::DestroyDownloadSession(const u96 index)
    {
        if (http_download_session.find(index) == http_download_session.end())
        {
            http_download_session.erase(index);
            return true;
        }
        return true;
    }

    HttpDownloadSession* HttpDownloadService::GetDownloadSession(const u96 index)
    {
        return http_download_session[index].get();
    }
}
