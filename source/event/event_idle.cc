#include <event_idle.h>
#include <event_loop.h>
namespace Evpp
{
    EventIdle::EventIdle(EventLoop* loop) :
        event_base(loop),
        event_idle_handler(new event_idle())
    {
        if (nullptr == event_idle_handler->data)
        {
            event_idle_handler->data = this;
        }
    }

    EventIdle::~EventIdle()
    {
        if (nullptr != event_idle_handler)
        {
            delete event_idle_handler;
            event_idle_handler = nullptr;
        }
    }

    bool EventIdle::CreaterIdle()
    {
        if (nullptr != event_base && nullptr != event_idle_handler)
        {
            return 0 == uv_idle_start(event_idle_handler, &EventIdle::OnNotify);
        }
        return false;
    }

    bool EventIdle::DestroyIdle()
    {
        if (nullptr != event_base && nullptr != event_idle_handler)
        {
            return 0 == uv_idle_stop(event_idle_handler);
        }
        return false;
    }

    void EventIdle::SetCallback(const EventLoopHandler& callback)
    {
        if (nullptr == event_idle_callback)
        {
            event_idle_callback = callback;
        }
    }

    bool EventIdle::InitialIdle()
    {
        if (nullptr != event_base && nullptr != event_idle_handler)
        {
            return 0 == uv_idle_init(event_base->EventBasic(), event_idle_handler);
        }
        return false;
    }

    void EventIdle::OnNotify(event_idle* handler)
    {
        if (nullptr != handler)
        {
            EventIdle* watcher = static_cast<EventIdle*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnNotify();
                }
            }
        }
    }

    void EventIdle::OnNotify()
    {
        if (nullptr != event_base && nullptr != event_idle_handler)
        {
            if (nullptr != event_idle_callback)
            {
                event_idle_callback(event_base);
            }
        }
    }
}
