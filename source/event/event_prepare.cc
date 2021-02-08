#include <event_prepare.h>
#include <event_loop.h>
namespace Evpp
{
    EventPrepare::EventPrepare(EventLoop* loop) :
        event_base(loop),
        event_prepare_handler(new event_prepare())
    {
        if (nullptr == event_prepare_handler->data)
        {
            event_prepare_handler->data = this;
        }
    }

    EventPrepare::~EventPrepare()
    {
        if (nullptr != event_prepare_handler)
        {
            delete event_prepare_handler;
            event_prepare_handler = nullptr;
        }
    }

    bool EventPrepare::CreaterPrepare()
    {
        if (nullptr != event_base && nullptr != event_prepare_handler)
        {
            return 0 == uv_prepare_start(event_prepare_handler, &EventPrepare::OnNotify);
        }
        return false;
    }

    bool EventPrepare::DestroyPrepare()
    {
        if (nullptr != event_base && nullptr != event_prepare_handler)
        {
            return 0 == uv_prepare_stop(event_prepare_handler);
        }
        return false;
    }

    void EventPrepare::SetCallback(const EventLoopHandler& callback)
    {
        if (nullptr != event_base && nullptr != event_prepare_handler)
        {
            event_prepare_callback = callback;
        }
    }

    bool EventPrepare::InitialPrepare()
    {
        if (nullptr != event_base && nullptr != event_prepare_handler)
        {
            return 0 == uv_prepare_init(event_base->EventBasic(), event_prepare_handler);
        }
        return false;
    }

    void EventPrepare::OnNotify(event_prepare* handler)
    {
        if (nullptr != handler)
        {
            EventPrepare* watcher = static_cast<EventPrepare*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnNotify();
                }
            }
        }
    }

    void EventPrepare::OnNotify()
    {
        if (nullptr != event_base && nullptr != event_prepare_handler)
        {
            if (nullptr != event_prepare_callback)
            {
                event_prepare_callback(event_base);
            }
        }
    }
}
