#ifndef __EVENT_TIMER_H__
#define __EVENT_TIMER_H__
#include <event_config.h>
namespace Evpp
{
    class EventLoop;
    class EventTimer : public EventStatus, public std::enable_shared_from_this<EventTimer>
    {
    public:
        explicit EventTimer(EventLoop* loop, const u96 index = 0);
        explicit EventTimer(EventLoop* loop, const EventTimerHandle& callback, const u96 index = 0);
        virtual ~EventTimer();
    public:
        bool InitedTimer();
        bool AssignTimer(const std::chrono::nanoseconds& delay, const std::chrono::nanoseconds& repeat);
        bool AssignTimer(const std::chrono::microseconds& delay, const std::chrono::microseconds& repeat);
        bool AssignTimer(const std::chrono::milliseconds& delay, const std::chrono::milliseconds& repeat);
        bool AssignTimer(const std::chrono::seconds& delay, const std::chrono::seconds& repeat);
        bool AssignTimer(const std::chrono::minutes& delay, const std::chrono::minutes& repeat);
        bool AssignTimer(const std::chrono::hours& delay, const std::chrono::hours& repeat);
        bool AssignTimer(const struct timeval& delay, const struct timeval& repeat);
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
        const u96 GetTimerIndex() { return timer_index; };
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
        EventLoop* event_base;
        event_timer* event_time;
        u96                                                         timer_index;
        EventTimerHandle                                            event_callback;
    };
}
#endif // __EVENT_TIMER_H__
