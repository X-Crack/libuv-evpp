#pragma once
#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__
#include <config.h>
#include <event_status.h>
#include <memory>
#include <atomic>
namespace Evpp
{
    class EventQueue;
    class EventLoop : public EventStatus, public std::enable_shared_from_this<EventLoop>
    {
    public:
        explicit EventLoop(event_loop* loop = uv_default_loop(), const u96 index = 0);
        virtual ~EventLoop();
    public:
        bool InitialEvent();
    public:
        bool ExitDispatch();
        bool StopDispatch();
    public:
        bool RunInLoop(const Functor& function);
    public:
        bool SelftyThread();
        i32  GetCurThread();
        event_loop* EventBasic() { return event_base; };
    public:
        u96 GetEventIndex() { return event_index; };
        EventLoop* AddRefer();
    private:
        event_loop*                                     event_base;
        u96                                             event_index;
        std::atomic<u32>                                event_refer;        // 当前 Loop session 个数 -> 用于动态线程判断分配新的线程使用
        std::unique_ptr<EventQueue>                     event_queue;
        i32                                             safe_index;
    };
}
#endif // __EVENT_LOOP_H__
