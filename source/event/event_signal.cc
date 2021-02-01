#include <event_signal.h>
#include <event_loop.h>
namespace Evpp
{
    EventSignal::EventSignal(EventLoop* loop, const Handler& function) : 
        event_base(loop), 
        event_signaler(new event_signal()),
        event_callback(function)
    {
        if (nullptr == event_signaler->data)
        {
            event_signaler->data = this;
        }
    }

    EventSignal::~EventSignal()
    {
        if (nullptr != event_signaler)
        {
            delete event_signaler;
            event_signaler = nullptr;
        }

        LOG_INFO << "Release Class EventSignal";
    }

    bool EventSignal::InitialSignal()
    {
        if (nullptr != event_base && nullptr != event_signaler)
        {
            return 0 == uv_signal_init(event_base->EventBasic(), event_signaler);
        }
        return false;
    }

    bool EventSignal::CreaterSignal(const i32 signum)
    {
        if (nullptr != event_base && nullptr != event_signaler)
        {
            return 0 == uv_signal_start(event_signaler, &EventSignal::OnNotify, signum);
        }
        return false;
    }

    bool EventSignal::DestroySignal()
    {
        if (nullptr != event_base && nullptr != event_signaler)
        {
            return 0 == uv_signal_stop(event_signaler);
        }
        return false;
    }

    void EventSignal::SetSignalCallback(const Handler& function)
    {
        if (nullptr == event_callback)
        {
            event_callback = function;
        }
    }

    void EventSignal::OnNotify(event_signal* handler, int signum)
    {
        if (nullptr != handler && signum > 0)
        {
            EventSignal* watcher = static_cast<EventSignal*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnNotify();
                }
            }
        }
    }

    void EventSignal::OnNotify()
    {
        if (nullptr != event_callback)
        {
            event_callback();
        }
    }
}