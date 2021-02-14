#include <event_async.h>
#include <event_loop.h>
namespace Evpp
{
    EventAsync::EventAsync(EventLoop* loop, const Handler& handler) :
        event_base(loop),
        event_queue(new event_async()),
        event_handler(handler)
    {
        if (nullptr == event_queue->data)
        {
            event_queue->data = this;
        }
    }

    EventAsync::~EventAsync()
    {

    }

    bool EventAsync::CreaterAsync()
    {
        if (nullptr != event_base && nullptr != event_queue)
        {
            return 0 == uv_async_init(event_base->EventBasic(), event_queue, &EventAsync::OnNotify);
        }
        return false;
    }

    bool EventAsync::DestroyAsync()
    {
        if (nullptr != event_base && nullptr != event_queue)
        {
            if (event_base->EventThread())
            {
                return SocketClose(event_queue, &EventAsync::DefaultClose);
            }
            return event_base->RunInLoopEx(std::bind(&EventAsync::DestroyAsync, this));
        }
        return false;
    }

    bool EventAsync::ExecNotify()
    {
        if (nullptr != event_queue && nullptr != event_queue->data)
        {
            if (SocketStatus(event_queue))
            {
                return 0 == uv_async_send(event_queue);
            }
        }
        return false;
    }

    void EventAsync::OnNotify(event_async* handler)
    {
        if (nullptr != handler)
        {
            EventAsync* watcher = static_cast<EventAsync*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnNotify();
                }
            }
        }
    }

    void EventAsync::OnNotify()
    {
        if (nullptr != event_handler)
        {
            event_handler();
        }
    }

    void EventAsync::DefaultClose(event_handle* handler)
    {
        if (nullptr != handler)
        {
            EventAsync* watcher = static_cast<EventAsync*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnClose(handler);
                }
            }
        }
    }

    void EventAsync::OnClose(event_handle* handler)
    {
        if (nullptr != handler)
        {
            delete handler;
            handler = nullptr;
        }
    }
}