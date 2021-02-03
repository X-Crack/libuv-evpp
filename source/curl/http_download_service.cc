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

    bool HttpDownloadService::CreaterDownload(EventLoop* loop, CURLM* curl_handler, const String* host, const u32 port)
    {
        if (loop->EventThread())
        {
            return CreaterDownload(loop, curl_handler, std::string(host), port);
        }
        return loop->RunInLoopEx(std::bind((bool(HttpDownloadService::*)(EventLoop*, CURLM*, const std::string&, const u32)) & HttpDownloadService::CreaterDownload, this, loop, curl_handler, host, port));
    }

    bool HttpDownloadService::CreaterDownload(EventLoop* loop, CURLM* curl_handler, const std::string& host, const u32 port)
    {
        if (loop->EventThread())
        {
            if (CreaterDownloadSession(host))
            {
                CURL* handler = curl_easy_init();
                curl_easy_setopt(handler, CURLOPT_URL, host.c_str());
                curl_easy_setopt(handler, CURLOPT_PORT, port);
                curl_easy_setopt(handler, CURLOPT_SSL_VERIFYHOST, 0L); // 关闭SSL
                curl_easy_setopt(handler, CURLOPT_SSL_VERIFYPEER, 0L); // 关闭SSL
                curl_easy_setopt(handler, CURLOPT_FOLLOWLOCATION, 1L); // 开启重定向

                curl_easy_setopt(handler, CURLOPT_NOPROGRESS, 0); // 设置进度响应

                curl_easy_setopt(handler, CURLOPT_PROGRESSDATA, GetDownloadSession());//数据传输的对象
                curl_easy_setopt(handler, CURLOPT_PROGRESSFUNCTION, &HttpDownloadSession::DefaultProgress);//进度响应函数

                curl_easy_setopt(handler, CURLOPT_WRITEDATA, GetDownloadSession());
                curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, &HttpDownloadSession::DefaultMessage);

                return CURLMcode::CURLM_OK == curl_multi_add_handle(curl_handler, handler);
            }
        }
        return loop->RunInLoopEx(std::bind((bool(HttpDownloadService::*)(EventLoop*, CURLM*, const std::string&, const u32)) & HttpDownloadService::CreaterDownload, this, loop, curl_handler, host, port));
    }

    bool HttpDownloadService::CreaterDownloadSession(const std::string& host)
    {
        if (nullptr == http_download_session)
        {
            http_download_session = std::make_shared<HttpDownloadSession>(host);
            return true;
        }
        return false;
    }

    HttpDownloadSession* HttpDownloadService::GetDownloadSession()
    {
        return http_download_session.get();
    }

    bool HttpDownloadService::CreaterDownloadPoll(EventLoop* loop, CURLM* curl_handler, const i32 fd)
    {
        if (nullptr == http_download_poll)
        {
            http_download_poll = std::make_shared<HttpDownloadPoll>(loop, curl_handler, fd);
            return true;
        }
        return true;
    }

    HttpDownloadPoll* HttpDownloadService::GetDownloadPoll()
    {
        return http_download_poll.get();
    }

    bool HttpDownloadService::DestroyPoll()
    {
        if (0 == uv_poll_stop(http_download_poll->GetPollHandler()))
        {
            return true;
        }
        return false;
    }
}
