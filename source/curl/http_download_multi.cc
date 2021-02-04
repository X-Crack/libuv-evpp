#include <http_download_multi.h>
#include <http_download_session.h>
#include <http_download_service.h>
#include <http_download_poll.h>
#include <event_share.h>
#include <event_loop.h>
#include <event_loop_thread_pool.h>
#include <event_timer.h>
namespace Evpp
{
    HttpDownloadMulti::HttpDownloadMulti(EventLoop* loop) :
        event_base(loop),
        event_timer(nullptr),
        http_download_service(std::make_unique<HttpDownloadService>()),
        http_curl_global_handler(nullptr),
        http_curl_global_handles(0)
    {

    }

    HttpDownloadMulti::~HttpDownloadMulti()
    {
        if (nullptr != http_curl_global_handler)
        {
            if (CURLMcode::CURLM_OK != curl_multi_cleanup(http_curl_global_handler))
            {
                EVENT_INFO("cleanup curl error");
            }
        }
    }

    bool HttpDownloadMulti::InitialDownload(CURLM* multi)
    {
        if (nullptr != event_base)
        {
            if (nullptr == http_curl_global_handler)
            {
                http_curl_global_handler = multi;
            }

            if (nullptr == event_timer)
            {
                event_timer.reset(new EventTimer(event_base, std::bind(&HttpDownloadMulti::OnTaskTimer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
            }

            if (nullptr != http_curl_global_handler)
            {
                return
                    CURLMcode::CURLM_OK == curl_multi_setopt(http_curl_global_handler, CURLMOPT_SOCKETDATA, this) &&
                    CURLMcode::CURLM_OK == curl_multi_setopt(http_curl_global_handler, CURLMOPT_TIMERDATA, this) &&
                    CURLMcode::CURLM_OK == curl_multi_setopt(http_curl_global_handler, CURLMOPT_SOCKETFUNCTION, &HttpDownloadMulti::DefaultSocket) &&
                    CURLMcode::CURLM_OK == curl_multi_setopt(http_curl_global_handler, CURLMOPT_TIMERFUNCTION, &HttpDownloadMulti::DefaultTimer);
            }
        }
        return false;
    }

    bool HttpDownloadMulti::CreaterDownload(const u96 index, const String* host, const u32 port)
    {
        return http_download_service->CreaterDownload(index, event_base, http_curl_global_handler, host, port);
    }

    bool HttpDownloadMulti::CreaterDownload(const u96 index, const std::string& host, const u32 port)
    {
        return http_download_service->CreaterDownload(index, event_base, http_curl_global_handler, host, port);
    }

    void HttpDownloadMulti::OnTaskTimer(EventLoop* loop, const std::shared_ptr<EventTimer>& timer, const u96 index)
    {
        USE_PARAMETER(loop);
        USE_PARAMETER(index);
        {
            if (timer->StopedTimer())
            {
                if (CURLMcode::CURLM_OK != curl_multi_socket_action(http_curl_global_handler, CURL_SOCKET_TIMEOUT, 0, &http_curl_global_handles))
                {
                    EVENT_INFO("An unexpected error occurred during the initial curl download task");
                }
            }
        }
    }

    i32 HttpDownloadMulti::OnSocket(CURL* easy, curl_socket_t fd, i32 action, HttpDownloadPoll* http_download_poll)
    {
        if (nullptr == http_download_poll)
        {
            http_download_poll = new HttpDownloadPoll(event_base, http_curl_global_handler, fd);
        }
        return InitialSocket(easy, fd, action, http_download_poll);
    }

    i32 HttpDownloadMulti::InitialSocket(CURL* easy, curl_socket_t fd, i32 action, HttpDownloadPoll* http_download_poll)
    {
        switch (action)
        {
        case CURL_POLL_IN:
        {
            uv_poll_start(http_download_poll->http_event_poll, UV_READABLE, &HttpDownloadPoll::DefaultCurlPerform);
            break;
        }
        case CURL_POLL_OUT:
        {
            uv_poll_start(http_download_poll->http_event_poll, UV_WRITABLE, &HttpDownloadPoll::DefaultCurlPerform);
            break;
        }
        case CURL_POLL_INOUT:
        {
            break;
        }
        case CURL_POLL_REMOVE:
        {
            if (0 == uv_poll_stop(http_download_poll->http_event_poll))
            {
                uv_close(reinterpret_cast<uv_handle_t*>(http_download_poll->http_event_poll), &HttpDownloadPoll::DefaultClose);
            }

            if (CURLMcode::CURLM_OK != curl_multi_assign(http_curl_global_handler, fd, nullptr))
            {
                EVENT_INFO("an exception occurred when removing the queue event");
            }
        }
        }
        return 0;
    }

    i32 HttpDownloadMulti::OnTimer(CURLM* multi, i32 delay)
    {
        if (delay <= 0)
        {
            delay = 1;
        }

        if (delay != ~0)
        {
            event_timer->AssignTimer(delay, 0);
        }
        else
        {
            event_timer->StopedTimer();
        }
        return 0;
    }

    i32 HttpDownloadMulti::DefaultSocket(CURL* easy, curl_socket_t fd, i32 action, void* handler, void* socketp)
    {
        if (nullptr != handler)
        {
            HttpDownloadMulti* watcher = static_cast<HttpDownloadMulti*>(handler);
            {
                if (nullptr != watcher)
                {
                    return watcher->OnSocket(easy, fd, action, static_cast<HttpDownloadPoll*>(socketp));
                }
            }
        }
        return 0;
    }

    i32 HttpDownloadMulti::DefaultTimer(CURLM* multi, i32 timeout_ms, void* handler)
    {
        if (nullptr != handler)
        {
            HttpDownloadMulti* watcher = static_cast<HttpDownloadMulti*>(handler);
            {
                if (nullptr != watcher)
                {
                    return watcher->OnTimer(multi, timeout_ms);
                }
            }
        }
        return 0;
    }
}
