#pragma once
#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__
#include <config.h>
#include <event_status.h>
#include <memory>
#include <atomic>
#include <any>
namespace Evpp
{
    class EventWatcher;
    class EventTimerPool;
    class EventLoop : public EventStatus, public std::enable_shared_from_this<EventLoop>
    {
    public:
        explicit EventLoop(event_loop* loop = uv_default_loop(), const u96 index = 0);
        virtual ~EventLoop();
    public:
        bool InitialEvent();
    public:
        bool ExecDispatch();
        bool ExecDispatch(u32 mode);
        bool ExecDispatch(const EventLoopHandler& function, u32 mode = UV_RUN_ONCE);
        bool ExecDispatchEx(const EventLoopHandler& function, u32 mode = UV_RUN_ONCE);
        bool StopDispatch();
    public:
        bool RunInLoop(const Functor& function);
        bool RunInLoop(Functor&& function);
        bool RunInLoopEx(const Handler& function);
        bool RunInLoopEx(Handler&& function);
    public:
        bool AssignTimer(const u96 index, const u64 delay, const u64 repeat);
        bool StopedTimer(const u96 index);
        void KilledTimer(const u96 index);
        void ModiyRepeat(const u96 index, const u64 repeat);
        bool ReStarTimer(const u96 index);
        bool ReStarTimerEx(const u96 index, const u64 delay, const u64 repeat);
    public:
        bool AddContext(const u96 index, const std::any& any);
        const std::unique_ptr<std::any>& GetContext(const u96 index = 0);
    public:
        bool EventThread();
        u32  EventThreadId();
        u32  EventThreadSelf();
    public:
        event_loop* EventBasic() { return event_base; };
    public:
        u96 GetEventIndex() { return event_index; };
        EventLoop* AddRefer();
    private:
        event_loop*                                             event_base;
        u96                                                     event_index;
        std::atomic<u32>                                        event_refer;        // 当前 Loop session 个数 -> 用于动态线程判断分配新的线程使用
        std::unique_ptr<EventWatcher>                           event_watcher;
        std::unique_ptr<EventTimerPool>                         event_timer_pool;
        std::unordered_map<u96, std::unique_ptr<std::any>>      event_context;
        u32                                                     event_thread;
        std::atomic<u32>                                        event_stop_flag;
    };
}
#endif // __EVENT_LOOP_H__
