#include <event_check.h>
#include <event_loop.h>
namespace Evpp
{
    EventCheck::EventCheck(EventLoop* loop) :
        event_base(loop),
        event_check_handler(new event_check())
    {
        if (nullptr == event_check_handler->data)
        {
            event_check_handler->data = this;
        }
    }

    EventCheck::~EventCheck()
    {
        if (nullptr != event_check_handler)
        {
            delete event_check_handler;
            event_check_handler = nullptr;
        }
    }

    bool EventCheck::CreaterCheck()
    {
        if (nullptr != event_base && nullptr != event_check_handler)
        {
            return 0 == uv_check_start(event_check_handler, &EventCheck::OnNotify);
        }
    }

    bool EventCheck::DestroyCheck()
    {
        if (nullptr != event_base && nullptr != event_check_handler)
        {
            return 0 == uv_check_stop(event_check_handler);
        }
        return false;
    }

    void EventCheck::SetCallback(const EventLoopHandler& callback)
    {
        if (nullptr == event_check_callback)
        {
            event_check_callback = callback;
        }

    }

    bool EventCheck::InitialCheck()
    {
        if (nullptr != event_base && nullptr != event_check_handler)
        {
            return 0 == uv_check_init(event_base->EventBasic(), event_check_handler);
        }
        return false;
    }

    void EventCheck::OnNotify(event_check* handler)
    {
        if (nullptr != handler)
        {
            EventCheck* watcher = static_cast<EventCheck*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnNotify();
                }
            }
        }
    }

    void EventCheck::OnNotify()
    {
        if (nullptr != event_base && nullptr != event_check_handler)
        {
            if (nullptr != event_check_callback)
            {
                event_check_callback(event_base);
            }
        }
    }
}
