#include <event_status.h>
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
        timer_index(index),
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
        if (KilledTimer())
        {
            if (nullptr != event_time->data)
            {
                event_time->data = nullptr;
            }
        }
    }

    bool EventTimer::InitedTimer()
    {
        if (nullptr != event_base && nullptr != event_time)
        {
            if (0 == uv_timer_init(event_base->EventBasic(), event_time))
            {
                return ChangeStatus(Status::None, Status::Init);
            }
        }
        return false;
    }

    template <class _Rep, class _Period>
    bool EventTimer::AssignTimer(const std::chrono::duration<_Rep, _Period>& delay, const std::chrono::duration<_Rep, _Period>& repeat)
    {
        return AssignTimer(static_cast<u64>(delay.count()), static_cast<u64>(repeat.count()));
    }

    bool EventTimer::AssignTimer(const std::chrono::nanoseconds& delay, const std::chrono::nanoseconds& repeat)
    {
        return AssignTimer(static_cast<u64>(delay.count()), static_cast<u64>(repeat.count()));
    }

    bool EventTimer::AssignTimer(const std::chrono::microseconds& delay, const std::chrono::microseconds& repeat)
    {
        return AssignTimer(static_cast<u64>(delay.count()), static_cast<u64>(repeat.count()));
    }

    bool EventTimer::AssignTimer(const std::chrono::milliseconds& delay, const std::chrono::milliseconds& repeat)
    {
        return AssignTimer(static_cast<u64>(delay.count()), static_cast<u64>(repeat.count()));
    }

    bool EventTimer::AssignTimer(const std::chrono::seconds& delay, const std::chrono::seconds& repeat)
    {
        return AssignTimer(static_cast<u64>(delay.count()), static_cast<u64>(repeat.count()));
    }

    bool EventTimer::AssignTimer(const std::chrono::minutes& delay, const std::chrono::minutes& repeat)
    {
        return AssignTimer(static_cast<u64>(delay.count()), static_cast<u64>(repeat.count()));
    }

    bool EventTimer::AssignTimer(const std::chrono::hours& delay, const std::chrono::hours& repeat)
    {
        return AssignTimer(static_cast<u64>(delay.count()), static_cast<u64>(repeat.count()));
    }

    bool EventTimer::AssignTimer(const struct timeval& delay, const struct timeval& repeat)
    {
        return AssignTimer(static_cast<u64>(std::chrono::duration_cast<std::chrono::milliseconds>(delay).count()), static_cast<u64>(std::chrono::duration_cast<std::chrono::milliseconds>(repeat).count()));
    }

    bool EventTimer::AssignTimer(const u64 delay, const u64 repeat)
    {
        if (nullptr != event_base && nullptr != event_time)
        {
            if (ExistsInited() || ExistsStoped())
            {
                if (uv_is_active(reinterpret_cast<event_handle*>(event_time)) || uv_timer_start(event_time, &EventTimer::OnNotify, delay, repeat))
                {
                    return false;
                }
                return ChangeStatus(Status::Exec);
            }
        }
        return false;
    }

    bool EventTimer::StopedTimer()
    {
        if (nullptr != event_base && nullptr != event_time)
        {
            if (ExistsRuning())
            {
                if (CheckStatus())
                {
                    if (0 == uv_timer_stop(event_time))
                    {
                        return ChangeStatus(Status::Stop);
                    }
                }
            }
        }
        return false;
    }

    bool EventTimer::KilledTimer()
    {
        if (ExistsStoped())
        {
            if (ChangeStatus(Status::Stop, Status::Exit))
            {
                return SocketClose(event_time, &EventTimer::DefaultClose);
            }
        }
        return false;
    }

    void EventTimer::ModiyRepeat(const u64 repeat)
    {
        uv_timer_set_repeat(event_time, repeat);
    }

    bool EventTimer::ReStarTimer()
    {
        if (nullptr != event_base && nullptr != event_time)
        {
            if (ExistsRuning() || ExistsStoped())
            {
                return 0 == uv_timer_again(event_time);
            }
        }
        return false;
    }

    bool EventTimer::ReStarTimerEx(const u64 delay, const u64 repeat)
    {
        if (nullptr != event_base && nullptr != event_time)
        {
            if (ExistsRuning() || ExistsStoped())
            {
                if (StopedTimer())
                {
                    return AssignTimer(delay, repeat);
                }
            }
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
            delete reinterpret_cast<event_timer*>(handler);
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
            event_callback(event_base, shared_from_this(), timer_index);
        }
    }
}
