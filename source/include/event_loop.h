﻿#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__
#include <event_config.h>
#include <event_status.h>
#include <event_mutex.h>
#include <memory>
#include <atomic>
#include <mutex>
#include <any>
namespace Evpp
{
    class EventQueue;
    class EventWatcher;
    class EventTimerPool;
    class EVPP_EXPORT EventLoop : public EventStatus, public std::enable_shared_from_this<EventLoop>
    {
    public:
        explicit EventLoop(event_loop* loop = uv_default_loop(), const u96 index = 0);
        virtual ~EventLoop();
    public:
        bool InitialEvent();
    public:
        bool ExecDispatch(i32 mode = UV_RUN_DEFAULT);
        // TODO: TranslateMessage DispatchMessage
        bool ExecDispatch(const EventLoopHandler& function, i32 mode = UV_RUN_ONCE);
        bool ExecDispatchEx(const EventLoopHandler& function, i32 mode = UV_RUN_ONCE);
        bool StopDispatch();
    public:
        bool RunInLoop(const Handler& function);
        bool RunInLoopEx(const Handler& function);
        bool RunInQueue(const Handler& function);
    public:
        bool AssignTimer(const u96 index, const u64 delay, const u64 repeat);
        bool StopedTimer(const u96 index);
        bool KilledTimer(const u96 index);
        void ModiyRepeat(const u96 index, const u64 repeat);
        bool ReStarTimer(const u96 index);
        bool ReStarTimerEx(const u96 index, const u64 delay, const u64 repeat);
    public:
        bool AddContext(const u96 index, const std::any& any);
        const std::unique_ptr<std::any>& GetContext(const u96 index = 0);
    public:
        bool EventThread();
        static u32  EventThreadId();
        u32  EventThreadSelf();
    public:
        event_loop* EventBasic();
    public:
        u96 GetEventIndex() { return event_index; };
        EventLoop* AddRefer();
    private:
        NOFORCEINLINE bool ExecDispatchEvent(i32 mode);
        NOFORCEINLINE bool SwitchDispatch();
        static void ObserveHandler(event_handle* handler, void* arg);
    private:
        event_loop*                                                         event_base;
        u96                                                                 event_index;
        std::atomic<u32>                                                    event_refer;        // 当前 Loop session 个数 -> 用于动态线程判断分配新的线程使用
        std::unique_ptr<EventQueue>                                         event_queue;
        std::unique_ptr<EventTimerPool>                                     event_timer_pool;
        std::unordered_map<u96, std::unique_ptr<std::any>>                  event_context;
        u32                                                                 event_thread;
        EventLocking                                                        event_locking;
    };
}
#endif // __EVENT_LOOP_H__
