#include <event_signal.h>
#include <event_loop.h>
namespace Evpp
{
    EventSignal::EventSignal(EventLoop* loop) : event_loop(loop), event_signaler(new event_signal())
    {
        if (nullptr == event_signaler->data)
        {
            event_signaler->data = this;
        }
    }

    EventSignal::~EventSignal()
    {

    }

    bool EventSignal::InitialSignal()
    {
        if (nullptr != event_loop && nullptr != event_signaler)
        {
            return 0 == uv_signal_init(event_loop->EventBasic(), event_signaler);
        }
        return false;
    }

    bool EventSignal::CreaterSignal(const i32 signum)
    {
        if (nullptr != event_loop && nullptr != event_signaler)
        {
            return 0 == uv_signal_start(event_signaler, &EventSignal::OnNotify, signum);
        }
        return false;
    }

    bool EventSignal::DestroySignal()
    {
        if (nullptr != event_loop && nullptr != event_signaler)
        {
            return 0 == uv_signal_stop(event_signaler);
        }
        return false;
    }

    void EventSignal::OnNotify(event_signal* handler, int signum)
    {
        if (signum > 0)
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
        printf("On Signal\n");
    }
}