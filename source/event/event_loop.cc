#include <event_loop.h>
#include <event_queue.h>
#include <event_timer_vesse.h>
namespace Evpp
{
    EventLoop::EventLoop(event_loop* loop, const u96 index) :
        event_base(loop),
        event_index(index),
        event_refer(0),
        event_queue(std::make_unique<EventQueue>(this)),
        event_timer_vesse(std::make_unique<EventTimerVesse>(this)),
        safe_index(GetCurrentThreadId())
    {
        if (ChangeStatus(NOTYET, INITIALIZING))
        {
            if (0 == uv_loop_init(event_base))
            {
                if (ChangeStatus(INITIALIZING, INITIALIZED))
                {
                    if (event_base)
                    {
                        uv_loop_set_data(event_base, this);
                    }
                }
            }
        }
    }

    EventLoop::~EventLoop()
    {
        uv_loop_close(event_base);
    }

    bool EventLoop::InitialEvent()
    {
        return event_queue->CreateQueue();
    }

    bool EventLoop::ExecDispatch()
    {
        if (0 != event_base)
        {
            if (ChangeStatus(INITIALIZED, RUNNING))
            {
                if (0 == uv_run(event_base, UV_RUN_DEFAULT))
                {
                    return ChangeStatus(RUNNING, STOPPED);
                }
            }
        }
        return false;
    }

    bool EventLoop::StopDispatch()
    {
        if (this->SelftyThread())
        {
            if (0 == event_base->stop_flag)
            {
                uv_stop(event_base);
                return true;
            }
            return false;
        }
        return RunInLoop(std::bind(&EventLoop::StopDispatch, this));
    }

    bool EventLoop::RunInLoop(const Functor& function)
    {
        if (nullptr == event_queue)
        {
            return false;
        }
        return event_queue->RunInLoop(function);
    }

    bool EventLoop::RunInLoop(Functor&& function)
    {
        return event_queue->RunInLoop(std::move(function));
    }

    bool EventLoop::AssignTimer(const u96 index, const u64 delay, const u64 repeat)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->AssignTimer(index, delay, repeat);
        }
        return false;
    }

    bool EventLoop::StopedTimer(const u96 index)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->StopedTimer(index);
        }
        return false;
    }

    bool EventLoop::KilledTimer(const u96 index)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->KilledTimer(index);
        }
        return false;
    }

    void EventLoop::ModiyRepeat(const u96 index, const u64 repeat)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->ModiyRepeat(index, repeat);
        }
        return;
    }

    bool EventLoop::ReStarTimer(const u96 index)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->ReStarTimer(index);
        }
        return false;
    }

    bool EventLoop::ReStarTimerEx(const u96 index, const u64 delay, const u64 repeat)
    {
        if (nullptr != event_timer_vesse)
        {
            return event_timer_vesse->ReStarTimerEx(index, delay, repeat);
        }
        return false;
    }

    bool EventLoop::AddContext(const u96 index, const std::any& any)
    {
        return event_context.emplace(index, std::make_unique<std::any>(any)).second;
    }

    const std::unique_ptr<std::any>& EventLoop::GetContext(const u96 index)
    {
        return event_context[index];
    }

    bool EventLoop::SelftyThread()
    {
        return static_cast<i32>(GetCurrentThreadId()) == this->safe_index;
    }

    i32 EventLoop::GetCurThread()
    {
        return safe_index;
    }

    EventLoop* EventLoop::AddRefer()
    {
        ++event_refer;
        return this;
    }
}