#include <event_timer_vesse.h>
#include <event_timer.h>
#include <event_loop.h>

namespace Evpp
{
    EventTimerVesse::EventTimerVesse(EventLoop* loop) : event_base(loop)
    {

    }

    EventTimerVesse::~EventTimerVesse()
    {

    }

    bool EventTimerVesse::AssignTimer(const u96 index, const u64 delay, const u64 repeat)
    {
        if (CreateTimer(index))
        {
            return event_timer[index]->AssignTimer(delay, repeat);
        }
        return false;
    }

    bool EventTimerVesse::StopedTimer(const u96 index)
    {
        return event_timer[index]->StopedTimer();
    }

    bool EventTimerVesse::KilledTimer(const u96 index)
    {
        return event_timer[index]->KilledTimer();
    }

    void EventTimerVesse::ModiyRepeat(const u96 index, const u64 repeat)
    {
        return event_timer[index]->ModiyRepeat(repeat);
    }

    bool EventTimerVesse::ReStarTimer(const u96 index)
    {
        return event_timer[index]->ReStarTimer();
    }

    bool EventTimerVesse::ReStarTimerEx(const u96 index, const u64 delay, const u64 repeat)
    {
        return event_timer[index]->ReStarTimerEx(delay, repeat);
    }

    const u64 EventTimerVesse::GetTimerduein(const u96 index)
    {
        return event_timer[index]->GetTimerduein();
    }

    bool EventTimerVesse::CreateTimer(const u96 index)
    {
        return event_timer.emplace(index, std::make_shared<EventTimer>(event_base, index)).second;
    }
}