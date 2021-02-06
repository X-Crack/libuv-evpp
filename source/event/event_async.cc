#include <event_async.h>
#include <event_loop.h>
namespace Evpp
{
    EventAsync::EventAsync(EventLoop* loop, const Handler& handler) :
        event_base(loop),
        event_async_(new event_async()),
        cv_handler(handler)
    {
        if (nullptr == event_async_->data)
        {
            event_async_->data = this;
        }
    }

    EventAsync::~EventAsync()
    {
        if (event_async_)
        {
            delete event_async_;
            event_async_ = nullptr;
        }
    }

    bool EventAsync::CreaterAsync()
    {
        if (nullptr != event_base && nullptr != event_async_)
        {
            return 0 == uv_async_init(event_base->EventBasic(), event_async_, &EventAsync::OnNotify);
        }
        return false;
    }

    bool EventAsync::DestroyAsync()
    {
        if (nullptr != event_base && nullptr != event_async_)
        {
            uv_close(reinterpret_cast<event_handle*>(event_async_), 0);
            return true;
        }
        return false;
    }

    bool EventAsync::ExecNotify()
    {
        if (nullptr != event_async_ && nullptr != event_async_->data)
        {
            return 0 == uv_async_send(event_async_);
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
        if (nullptr != cv_handler)
        {
            cv_handler();
        }
    }
}
