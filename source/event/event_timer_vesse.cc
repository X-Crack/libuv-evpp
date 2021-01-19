#include <event_timer_vesse.h>
#include <event_timer.h>
#include <event_loop.h>

namespace Evpp
{
    EventTimerVesse::EventTimerVesse(EventLoop* loop) : event_loop(loop)
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

    bool EventTimerVesse::CreateTimer(const u96 index)
    {
        return event_timer.emplace(index, std::make_shared<EventTimer>(event_loop, index)).second;
    }
}