#ifndef __EVENT_TIMER_H__
#define __EVENT_TIMER_H__
#include <config.h>
namespace Evpp
{
    class EventLoop;
    class EventTimer
    {
    public:
        explicit EventTimer(EventLoop* loop, const u96 index = 0);
        virtual ~EventTimer();
    public:
        bool InitedTimer();
        bool AssignTimer(const u64 delay, const u64 repeat);
    public:
        bool StopedTimer();
        bool KilledTimer();
    public:
        void ModiyRepeat(const u64 repeat);
        bool ReStarTimer();
        bool ReStarTimerEx(const u64 delay, const u64 repeat);
    public:
        const u64 GetTimerduein();
        const u96 GetTimerIndex() { return timer_index; };
    private:
        static void OnNotify(event_timer* handler);
        void OnNotify();
    private:
        EventLoop*                                                  event_loop;
        event_timer*                                                event_time;
        u96                                                         timer_index;
    };
}
#endif // __EVENT_TIMER_H__