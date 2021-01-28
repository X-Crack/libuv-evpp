#include <event_loop.h>
#include <event_watcher.h>
#include <event_timer_vesse.h>
namespace Evpp
{
    EventLoop::EventLoop(event_loop* loop, const u96 index) :
        event_base(loop),
        event_index(index),
        event_refer(0),
        event_watcher(std::make_unique<EventWatcher>(this)),
        event_timer_vesse(std::make_unique<EventTimerVesse>(this)),
        event_thread(EventThreadId())
    {
        if (ChangeStatus(None, Init))
        {
            if (0 == uv_loop_init(event_base))
            {
                if (event_base)
                {
                    uv_loop_set_data(event_base, this);
                }
            }
        }
    }

    EventLoop::~EventLoop()
    {

    }

    bool EventLoop::InitialEvent()
    {
        return event_watcher->CreaterQueue();
    }

    bool EventLoop::ExecDispatch()
    {
        if (0 != event_base)
        {
            if (ChangeStatus(Init, Exec))
            {
                if (0 == uv_run(event_base, UV_RUN_DEFAULT))
                {
                    return ChangeStatus(Exec, Stop);
                }
            }
        }
        return false;
    }

    bool EventLoop::ExecDispatch(u32 mode)
    {
        if (0 != event_base)
        {
            return 0 == uv_run(event_base, static_cast<uv_run_mode>(mode));
        }
        return false;
    }

    bool EventLoop::ExecDispatch(const Handler& function, u32 mode)
    {
        if (0 != event_base)
        {
            for (; 0 == event_base->stop_flag && 1 == uv_run(event_base, static_cast<uv_run_mode>(mode));)
            {
                if (nullptr != function)
                {
                    function();
                }
            }
        }
        return false;
    }

    bool EventLoop::StopDispatch()
    {
        if (this->EventThread())
        {
            if (0 == event_base->stop_flag)
            {
                while (event_watcher->DestroyQueue())
                {
                    Sleep(1);
                }
                uv_stop(event_base);
                return true;
            }
            return false;
        }
        return RunInLoop(std::bind(&EventLoop::StopDispatch, this));
    }

    bool EventLoop::RunInLoop(const Functor& function)
    {
        if (nullptr == event_watcher)
        {
            return false;
        }
        return event_watcher->RunInLoop(function);
    }

    bool EventLoop::RunInLoop(Functor&& function)
    {
        return event_watcher->RunInLoop(std::move(function));
    }

    bool EventLoop::RunInLoopEx(const Handler& function)
    {
        if (nullptr == event_watcher)
        {
            return false;
        }
        return event_watcher->RunInLoopEx(function);
    }

    bool EventLoop::RunInLoopEx(Handler&& function)
    {
        return event_watcher->RunInLoopEx(std::move(function));
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

    bool EventLoop::EventThread()
    {
        return EventThreadId() == this->event_thread;
    }

    u32 EventLoop::EventThreadId()
    {
#ifdef H_OS_WINDOWS
        return GetCurrentThreadId();
#else
        return static_cast<u32>(uv_thread_self());
#endif
    }

    u32 EventLoop::EventThreadSelf()
    {
        return event_thread;
    }

    void EventLoop::SetEventThreadId(const u32 id)
    {
        event_thread = id;
    }

    EventLoop* EventLoop::AddRefer()
    {
        ++event_refer;
        return this;
    }
}