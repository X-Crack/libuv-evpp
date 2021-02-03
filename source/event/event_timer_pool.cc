#include <event_status.h>
#include <event_timer_pool.h>
#include <event_timer.h>
#include <event_loop.h>
namespace Evpp
{
    EventTimerPool::EventTimerPool(EventLoop* loop) : event_base(loop)
    {

    }

    EventTimerPool::~EventTimerPool()
    {

    }

    bool EventTimerPool::AssignTimer(const u96 index, const u64 delay, const u64 repeat)
    {
        if (CreateTimer(index))
        {
            return event_timer[index]->AssignTimer(delay, repeat);
        }
        return false;
    }

    bool EventTimerPool::StopedTimer(const u96 index)
    {
        return event_timer[index]->StopedTimer();
    }

    void EventTimerPool::KilledTimer(const u96 index)
    {
        return event_timer[index]->KilledTimer();
    }

    void EventTimerPool::ModiyRepeat(const u96 index, const u64 repeat)
    {
        return event_timer[index]->ModiyRepeat(repeat);
    }

    bool EventTimerPool::ReStarTimer(const u96 index)
    {
        return event_timer[index]->ReStarTimer();
    }

    bool EventTimerPool::ReStarTimerEx(const u96 index, const u64 delay, const u64 repeat)
    {
        return event_timer[index]->ReStarTimerEx(delay, repeat);
    }

    const u64 EventTimerPool::GetTimerduein(const u96 index)
    {
        return event_timer[index]->GetTimerduein();
    }

    bool EventTimerPool::CreateTimer(const u96 index)
    {
        return event_timer.emplace(index, std::make_shared<EventTimer>(event_base, index)).second;
    }
}