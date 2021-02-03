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
            if (CreaterDownloadSession(host))
            {
                CURL* handler = curl_easy_init();
                curl_easy_setopt(handler, CURLOPT_URL, host);
                curl_easy_setopt(handler, CURLOPT_PORT, port);
                curl_easy_setopt(handler, CURLOPT_SSL_VERIFYHOST, 0L);
                curl_easy_setopt(handler, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(handler, CURLOPT_WRITEDATA, GetDownloadSession());
                curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, &HttpDownloadSession::DefaultMessage);

                return CURLMcode::CURLM_OK == curl_multi_add_handle(curl_handler, handler);
            }
        }
        return loop->RunInLoopEx(std::bind(&HttpDownloadService::CreaterDownload, this, loop, curl_handler, host, port));
    }

    bool HttpDownloadService::CreaterDownloadSession(const String* host)
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