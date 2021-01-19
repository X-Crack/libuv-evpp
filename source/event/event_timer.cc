#include <event_timer.h>
#include <event_loop.h>
namespace Evpp
{
    EventTimer::EventTimer(EventLoop* loop, const u96 index) :
        event_loop(loop),
        event_time(new event_timer()),
        timer_index(index)
    {
        event_time->data = this;
        // TODO: Test
        this->InitedTimer();
    }

    EventTimer::~EventTimer()
    {
        if (nullptr != event_time)
        {
            delete event_time;
            event_time = nullptr;
        }
    }

    bool EventTimer::InitedTimer()
    {
        if (nullptr != event_loop && nullptr != event_time)
        {
            return 0 == uv_timer_init(event_loop->EventBasic(), event_time);
        }
        return false;
    }

    bool EventTimer::AssignTimer(const u64 delay, const u64 repeat)
    {
        if (nullptr != event_loop && nullptr != event_time)
        {
            return 0 == uv_timer_start(event_time, &EventTimer::OnNotify, delay, repeat);
        }
        return false;
    }

    bool EventTimer::StopedTimer()
    {
        if (nullptr != event_loop && nullptr != event_time)
        {
            if (uv_is_active(reinterpret_cast<event_handle*>(event_time)))
            {
                return 0 == uv_timer_stop(event_time);
            }
        }
        return false;
    }

    bool EventTimer::KilledTimer()
    {
        if (StopedTimer())
        {
            if (0 == uv_is_closing(reinterpret_cast<event_handle*>(event_time)))
            {
                uv_close(reinterpret_cast<event_handle*>(event_time), 0);
            }
            return true;
        }
        return false;
    }

    void EventTimer::ModiyRepeat(const u64 repeat)
    {
        uv_timer_set_repeat(event_time, repeat);
    }

    bool EventTimer::ReStarTimer()
    {
        if (nullptr != event_loop && nullptr != event_time)
        {
            return 0 == uv_timer_again(event_time);
        }
        return false;
    }

    bool EventTimer::ReStarTimerEx(const u64 delay, const u64 repeat)
    {
        if (StopedTimer())
        {
            return AssignTimer(delay, repeat);
        }
        return false;
    }

    const u64 EventTimer::GetTimerduein()
    {
        return uv_timer_get_due_in(event_time);
    }

    void EventTimer::OnNotify(event_timer* handler)
    {
        EventTimer* watcher = static_cast<EventTimer*>(handler->data);
        {
            if (nullptr != watcher)
            {
                watcher->OnNotify();
            }
        }
    }

    void EventTimer::OnNotify()
    {
        printf("On Timer %lld\n", GetTimerduein());
    }
}