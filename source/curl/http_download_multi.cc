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
        event_timer(std::make_shared<EventTimer>(loop, std::bind(&HttpDownloadMulti::OnTaskTimer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))),
        http_download_service(std::make_unique<HttpDownloadService>()),
        http_curl_global_handler(curl_multi_init()),
        http_curl_global_handles(0)
    {
        InitialDownload();
    }

    HttpDownloadMulti::~HttpDownloadMulti()
    {
        if (CURLMcode::CURLM_OK != curl_multi_cleanup(http_curl_global_handler))
        {
            EVENT_INFO("cleanup curl error");
        }
    }

    bool HttpDownloadMulti::InitialDownload()
    {
        if (nullptr != http_curl_global_handler)
        {
            curl_multi_setopt(http_curl_global_handler, CURLMOPT_SOCKETDATA, this);
            curl_multi_setopt(http_curl_global_handler, CURLMOPT_TIMERDATA, this);
            curl_multi_setopt(http_curl_global_handler, CURLMOPT_SOCKETFUNCTION, &HttpDownloadMulti::DefaultSocket);
            curl_multi_setopt(http_curl_global_handler, CURLMOPT_TIMERFUNCTION, &HttpDownloadMulti::DefaultTimer);
            return true;
        }
        return false;
    }

    bool HttpDownloadMulti::CreaterDownload(const String* host, const u32 port)
    {
        return http_download_service->CreaterDownload(event_base, http_curl_global_handler, host, port);
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

    i32 HttpDownloadMulti::OnSocket(CURL* easy, curl_socket_t fd, i32 action)
    {
        if (nullptr != easy && nullptr != easy->set.out)
        {
            return CreaterSocket(static_cast<HttpDownloadSession*>(easy->set.out), fd, action);
        }
        return 0;
    }

    i32 HttpDownloadMulti::CreaterSocket(HttpDownloadSession* session, curl_socket_t fd, i32 action)
    {
        if (nullptr != session)
        {
            if (http_download_service->CreaterDownloadPoll(event_base, http_curl_global_handler, fd))
            {
                return InitialSocket(http_download_service->GetDownloadPoll(), action);
            }
        }
        return 0;
    }

    i32 HttpDownloadMulti::InitialSocket(HttpDownloadPoll* poll, i32 action)
    {
        switch (action)
        {
        case CURL_POLL_IN:
        {
            if (uv_poll_start(poll->GetPollHandler(), UV_READABLE, &HttpDownloadPoll::DefaultCurlPerform))
            {
                EVENT_INFO("an unexpected error occurred when initializing the Poll read event.");
            }
            break;
        }
        case CURL_POLL_OUT:
        {
            if (uv_poll_start(poll->GetPollHandler(), UV_WRITABLE, &HttpDownloadPoll::DefaultCurlPerform))
            {
                EVENT_INFO("an unexpected error occurred when initializing the Poll write event.");
            }
            break;
        }
        case CURL_POLL_REMOVE:
        {
            if (http_download_service->DestroyPoll())
            {
                if (CURLMcode::CURLM_OK != curl_multi_assign(http_curl_global_handler, poll->GetPollSocket(), 0))
                {
                    // 移除发生异常可能远程端口被提前关闭
                    EVENT_INFO("an exception occurred when removing the queue event");
                }
            }
            break;
        }
        }
        return 0;
    }

    i32 HttpDownloadMulti::OnTimer(CURLM* multi, i32 delay)
    {
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
                    return watcher->OnSocket(easy, fd, action);
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