#include <event_timer.h>
#include <event_loop.h>
namespace Evpp
{
    EventTimer::EventTimer(EventLoop* loop, const u96 index) : EventTimer(loop, EventTimerHandle(), index)
    {

    }

    EventTimer::EventTimer(EventLoop* loop, const EventTimerHandle& callback, const u96 index) :
        event_base(loop),
        event_time(new event_timer()),
        safe_index(index),
        event_callback(callback)
    {
        if (nullptr == event_time->data)
        {
            event_time->data = this;
        }
        
        if (0 == InitedTimer())
        {
            EVENT_INFO("An unpredictable error occurred when initializing the timer, please follow the libuv method to use EventTimer");
        }
    }

    EventTimer::~EventTimer()
    {
        if (nullptr != event_time->data)
        {
            event_time->data = nullptr;
        }

        {
            KilledTimer();
        }
    }

    bool EventTimer::InitedTimer()
    {
        if (nullptr != event_base && nullptr != event_time)
        {
            return 0 == uv_timer_init(event_base->EventBasic(), event_time);
        }
        return false;
    }

    bool EventTimer::AssignTimer(const u64 delay, const u64 repeat)
    {
        if (nullptr != event_base && nullptr != event_time)
        {
            if (0 == uv_is_active(reinterpret_cast<event_handle*>(event_time)))
            {
                return 0 == uv_timer_start(event_time, &EventTimer::OnNotify, delay, repeat);
            }
        }
        return false;
    }

    bool EventTimer::StopedTimer()
    {
        if (nullptr != event_base && nullptr != event_time)
        {
            if (CheckStatus())
            {
                return 0 == uv_timer_stop(event_time);
            }
        }
        return false;
    }

    void EventTimer::KilledTimer()
    {
        if (StopedTimer())
        {
            return uv_close(reinterpret_cast<event_handle*>(event_time), &EventTimer::DefaultClose);
        }
    }

    void EventTimer::ModiyRepeat(const u64 repeat)
    {
        uv_timer_set_repeat(event_time, repeat);
    }

    bool EventTimer::ReStarTimer()
    {
        if (nullptr != event_base && nullptr != event_time)
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

    void EventTimer::SetEventTimerCallback(const EventTimerHandle& callback)
    {
        if (nullptr == event_callback)
        {
            event_callback = callback;
        }
    }

    bool EventTimer::CheckStatus()
    {
        if (nullptr != event_base && nullptr != event_time)
        {
            if (uv_is_active(reinterpret_cast<event_handle*>(event_time)))
            {
                if (uv_is_closing(reinterpret_cast<event_handle*>(event_time)))
                {
                    return true;
                }
            }
            return true;
        }
        return false;
    }

    void EventTimer::DefaultClose(event_handle* handler)
    {
        if (nullptr != handler)
        {
            delete handler;
            handler = nullptr;
        }
    }

    void EventTimer::OnNotify(event_timer* handler)
    {
        if (nullptr != handler)
        {
            EventTimer* watcher = static_cast<EventTimer*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnNotify();
                }
            }
        }
    }

    void EventTimer::OnNotify()
    {
        if (nullptr != event_callback)
        {
            event_callback(event_base, shared_from_this(), safe_index);
        }
    }
}