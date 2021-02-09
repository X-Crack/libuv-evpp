#ifndef __EVENT_TIMER_POOL_H__
#define __EVENT_TIMER_POOL_H__
#include <event_config.h>
#include <unordered_map>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class EventTimer;
    class EventTimerPool final
    {
    public:
        explicit EventTimerPool(EventLoop* loop);
        virtual ~EventTimerPool();
    public:
        bool AssignTimer(const u96 index, const u64 delay, const u64 repeat);
    public:
        bool StopedTimer(const u96 index);
        bool KilledTimer(const u96 index);
    public:
        void ModiyRepeat(const u96 index, const u64 repeat);
        bool ReStarTimer(const u96 index);
        bool ReStarTimerEx(const u96 index, const u64 delay, const u64 repeat);
    public:
        const u64 GetTimerduein(const u96 index);
    private:
        bool CreateTimer(const u96 index);
    private:
        EventLoop*                                                      event_base;
        std::unordered_map<u96, std::shared_ptr<EventTimer>>            event_timer;
    };
}
#endif // __EVENT_TIMER_POOL_H__
