#include <http_download_session.h>
namespace Evpp
{
    HttpDownloadSession::HttpDownloadSession(const u96 index, CURL* easy_cyrl, const std::string& hosts) :
        http_index(index),
        http_easy_curl(easy_cyrl),
        http_hosts(hosts),
        http_curl_hosts(nullptr),
        http_curl_response_code(0),
        original_download_size(0)
    {

    }

    HttpDownloadSession::~HttpDownloadSession()
    {

    }

    void HttpDownloadSession::SetMessageCallback(const CurlMessageHandler& message)
    {
        if (nullptr == http_curl_message)
        {
            http_curl_message = message;
        }
    }

    void HttpDownloadSession::SetProgressCallback(const CurlProgressHandler& progress)
    {
        if (nullptr == http_curl_progress)
        {
            http_curl_progress = progress;
        }
    }

    const std::string& HttpDownloadSession::GetDownloadHosts()
    {
        return http_hosts;
    }

    u96 HttpDownloadSession::OnMessage(void* buffer, u96 size, u96 nmemb)
    {
        if (nullptr != http_curl_message)
        {
            return http_curl_message(http_index, buffer, size, nmemb);
        }
        return size * nmemb;
    }

    i32 HttpDownloadSession::OnProgress(double count_download_size, double current_download_size, double count_upload_size, double current_upload_size)
    {
        if (CURLcode::CURLE_OK == curl_easy_getinfo(http_easy_curl, CURLINFO_RESPONSE_CODE, &http_curl_response_code))
        {
            switch (http_curl_response_code)
            {
            case 300:
            case 301:
            case 302:
            {
                return 0;
            }
            case 303:
            case 304:
            case 305:
            case 306:
            case 307:
            case 400:
            case 401:
            case 402:
            case 403:
            case 404:
            case 405:
            case 406:
            case 407:
            case 408:
            case 409:
            case 410:
            case 411:
            case 412:
            case 413:
            case 414:
            case 415:
            case 416:
            case 417:
            case 500:
            case 501:
            case 502:
            case 503:
            case 504:
            case 505:
            {
                return 1;
            }
            }

            if (200 == http_curl_response_code)
            {
                if (CURLcode::CURLE_OK == curl_easy_getinfo(http_easy_curl, CURLINFO_EFFECTIVE_URL, &http_curl_hosts))
                {
                    if (current_download_size == original_download_size)
                    {
                        return 0;
                    }
                    else
                    {
                        original_download_size = current_download_size;
                    }

                    if (0 == count_download_size)
                    {
                        count_download_size = current_download_size;
                    }

                    if (current_download_size > 0)
                    {
                        if (nullptr != http_curl_progress)
                        {
                            if (http_curl_progress(http_index, count_download_size, current_download_size, count_upload_size, current_upload_size))
                            {
                                return 1;
                            }
                        }
                        else
                        {
                            EVENT_INFO("下载地址:%s 当前下载:%.2F%%", http_curl_hosts, (current_download_size * 100.0F) / count_download_size);
                        }
                    }

                    if (0 != current_download_size && current_download_size >= count_download_size)
                    {
                        return 1;
                    }
                }
            }
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
