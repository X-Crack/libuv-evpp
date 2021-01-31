#ifndef __EVENT_TIMER_H__
#define __EVENT_TIMER_H__
#include <event_config.h>
namespace Evpp
{
    class EventLoop;
    class EventTimer : public std::enable_shared_from_this<EventTimer>
    {
    public:
        explicit EventTimer(EventLoop* loop, const u96 index = 0);
        explicit EventTimer(EventLoop* loop, const EventTimerHandle& callback, const u96 index = 0);
        virtual ~EventTimer();
    public:
        bool InitedTimer();
        bool AssignTimer(const u64 delay, const u64 repeat);
    public:
        bool StopedTimer();
        void KilledTimer();
    public:
        void ModiyRepeat(const u64 repeat);
        bool ReStarTimer();
        bool ReStarTimerEx(const u64 delay, const u64 repeat);
    public:
        const u64 GetTimerduein();
        const u96 GetTimerIndex() { return safe_index; };
    public:
        void SetEventTimerCallback(const EventTimerHandle& callback);
    private:
        bool CheckStatus();
    private:
        static void DefaultClose(event_handle* handler);
    private:
        static void OnNotify(event_timer* handler);
        void OnNotify();
    private:
        EventLoop*                                                  event_base;
        event_timer*                                                event_time;
        u96                                                         safe_index;
        EventTimerHandle                                            event_callback;
    };
}
#endif // __EVENT_TIMER_H__