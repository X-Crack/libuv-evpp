#include <http_download_poll.h>
#include <event_loop.h>
#include <event_timer.h>
namespace Evpp
{
    HttpDownloadPoll::HttpDownloadPoll(EventLoop* loop, CURLM* curl, const i32 fd) :
        event_base(loop),
        event_timer(std::make_shared<EventTimer>(loop, std::bind(&HttpDownloadPoll::OnTaskTimer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))),
        http_event_poll(new event_poll()),
        http_curl_handler(curl),
        http_fd(fd),
        http_curl_runed(0),
        http_curl_queue(0),
        http_curl_handles(0),
        http_curl_hosts(nullptr)
    {
        if (0 == uv_poll_init_socket(loop->EventBasic(), http_event_poll, fd) && CURLMcode::CURLM_OK == curl_multi_assign(http_curl_handler, fd, this))
        {
            if (nullptr == http_event_poll->data)
            {
                http_event_poll->data = this;
            }
        }
    }

    HttpDownloadPoll::~HttpDownloadPoll()
    {
        if (nullptr != http_event_poll)
        {
            delete http_event_poll;
            http_event_poll = nullptr;
        }
    }

    void HttpDownloadPoll::OnCurlPerform(uv_poll_t* req, int status, int events)
    {
        if (0 != status)
        {
            events = CURL_CSELECT_ERR;
        }

        if (0 == status && events & UV_READABLE)
        {
            events |= CURL_CSELECT_IN;
        }

        if (0 == status && events & UV_WRITABLE)
        {
            events |= CURL_CSELECT_OUT;
        }

        if (CURLMcode::CURLM_OK == curl_multi_socket_action(http_curl_handler, req->socket, events, &http_curl_runed))
        {
            DownloadMessage(curl_multi_info_read(http_curl_handler, &http_curl_queue));
        }
    }

    void HttpDownloadPoll::OnClose(event_handle* handler)
    {
        if (nullptr != handler)
        {
            delete reinterpret_cast<uv_handle_t*>(handler);
            handler = nullptr;
        }
    }

    void HttpDownloadPoll::DefaultCurlPerform(uv_poll_t* handler, int status, int events)
    {
        if (nullptr != handler)
        {
            HttpDownloadPoll* watcher = static_cast<HttpDownloadPoll*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    return watcher->OnCurlPerform(handler, status, events);
                }
            }
        }
    }

    void HttpDownloadPoll::DefaultClose(event_handle* handler)
    {
        if (nullptr != handler)
        {
            HttpDownloadPoll* watcher = static_cast<HttpDownloadPoll*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    return watcher->OnClose(handler);
                }
            }
        }
    }

    void HttpDownloadPoll::DownloadMessage(CURLMsg* message)
    {
        if (nullptr != message)
        {
            if (CURLMSG_DONE == message->msg)
            {
                if (CURLcode::CURLE_OK == curl_easy_getinfo(message->easy_handle, CURLINFO_EFFECTIVE_URL, &http_curl_hosts))
                {
                    if (CURLMcode::CURLM_OK == curl_multi_remove_handle(http_curl_handler, message->easy_handle))
                    {
                        EVENT_INFO("完成任务:%s", http_curl_hosts);
                        curl_easy_cleanup(message->easy_handle);
                        uv_close(reinterpret_cast<uv_handle_t*>(http_event_poll), &HttpDownloadPoll::DefaultClose);
                    }
                }
            }
        }
    }

    void HttpDownloadPoll::OnTaskTimer(EventLoop* loop, const std::shared_ptr<EventTimer>& timer, const u96 index)
    {
        USE_PARAMETER(loop);
        USE_PARAMETER(index);
        {
            if (timer->StopedTimer())
            {
                if (CURLMcode::CURLM_OK != curl_multi_socket_action(http_curl_handler, CURL_SOCKET_TIMEOUT, 0, &http_curl_handles))
                {
                    EVENT_INFO("An unexpected error occurred during the initial curl download task");
                }
            }
        }
    }
}
