#include <http_download_poll.h>
#include <event_loop.h>
#include <event_timer.h>
namespace Evpp
{
    HttpDownloadPoll::HttpDownloadPoll(EventLoop* loop, CURLM* curl_handler, const i32 fd, const CurlMessageTaskHandler& task_message) :
        event_base(loop),
        http_event_poll(new event_poll),
        http_curl_handler(curl_handler),
        http_fd(fd),
        http_curl_task_message(task_message),
        http_curl_queue(0),
        http_curl_handles(0),
        http_curl_hosts(0)
    {
        if (0 == uv_poll_init_socket(loop->EventBasic(), http_event_poll, fd) && 0 == curl_multi_assign(curl_handler, fd, this))
        {
            http_event_poll->data = this;
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

    void HttpDownloadPoll::SetTaskMessageCallback(const CurlMessageTaskHandler& task_message)
    {
        if (nullptr == http_curl_task_message)
        {
            http_curl_task_message = task_message;
        }
    }

    void HttpDownloadPoll::OnClose(HttpDownloadPoll* handler)
    {
        if (this == handler)
        {
            delete this;
        }
        else
        {
            delete handler;
        }
    }

    void HttpDownloadPoll::OnCurlPerform(event_poll* handler, int status, int events)
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

        if (CURLMcode::CURLM_OK == curl_multi_socket_action(http_curl_handler, handler->socket, events, &http_curl_handles))
        {
            if (nullptr != http_curl_task_message)
            {
                http_curl_task_message(event_base->GetEventIndex(), http_curl_handles);
            }

            DownloadMessage(curl_multi_info_read(http_curl_handler, &http_curl_queue), http_curl_handles);
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
                    return watcher->OnClose(watcher);
                }
            }
        }
    }

    void HttpDownloadPoll::DefaultCurlPerform(event_poll* handler, int status, int events)
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

    void HttpDownloadPoll::DownloadMessage(CURLMsg* message, int handles)
    {
        if (nullptr != message)
        {
            if (CURLMSG_DONE == message->msg)
            {
                if (CURLcode::CURLE_OK == curl_easy_getinfo(message->easy_handle, CURLINFO_EFFECTIVE_URL, &http_curl_hosts))
                {
                    if (CURLMcode::CURLM_OK == curl_multi_remove_handle(http_curl_handler, message->easy_handle))
                    {
                        if (nullptr != message->easy_handle)
                        {
                            EVENT_INFO("下载任务完成：%s", http_curl_hosts);
                            curl_easy_cleanup(message->easy_handle);
                        }
                    }
                }
            }
        }
    }
}
