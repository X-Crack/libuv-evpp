#include <event_loop.h>
#include <event_watcher.h>
#include <event_timer_pool.h>
namespace Evpp
{
    EventLoop::EventLoop(event_loop* loop, const u96 index) :
        event_base(loop),
        event_index(index),
        event_refer(0),
        event_watcher(std::make_unique<EventWatcher>(this)),
        event_timer_pool(std::make_unique<EventTimerPool>(this)),
        event_thread(0),
        event_stop_flag(1)
    {

    }

    EventLoop::~EventLoop()
    {

    }

    bool EventLoop::InitialEvent()
    {
        if (ChangeStatus(Status::None, Status::Init))
        {
            if (0 == uv_loop_init(event_base))
            {
                if (event_base)
                {
                    uv_loop_set_data(event_base, this);
                }

                if (0 == event_thread)
                {
                    event_thread = EventThreadId();
                }
                return event_watcher->CreaterQueue();
            }

            EVENT_INFO("an error occurred during the initialization loop this: %p", this);
        }
        return false;
    }

    bool EventLoop::ExecDispatch()
    {
        if (nullptr != event_base)
        {
            if (ChangeStatus(Status::Init, Status::Exec))
            {
                if (0 == ExecDispatch(UV_RUN_DEFAULT))
                {
                    return ChangeStatus(Status::Exec, Status::Stop);
                }

                EVENT_INFO("the cyclic event stopped running and an unexpected error occurred this: %p", this);

                assert(0);
            }
        }
        return false;
    }

    bool EventLoop::ExecDispatch(u32 mode)
    {
        if (nullptr != event_base)
        {
            if (ExistsInited())
            {
                ChangeStatus(Status::Exec);
            }

            return 0 == uv_run(event_base, static_cast<uv_run_mode>(mode));
        }
        return false;
    }

    bool EventLoop::ExecDispatch(const EventLoopHandler& function, u32 mode)
    {
        if (nullptr != event_base)
        {
            if (0 == ExecDispatch(mode))
            {
                if (nullptr != function)
                {
                    function(this);
                }
            }
            return true;
        }
        return false;
    }


    bool EventLoop::ExecDispatchEx(const EventLoopHandler& function, u32 mode)
    {
        if (nullptr != event_base)
        {
            if (ChangeStatus(Status::Init, Status::Exec))
            {
                for (; 0 == event_base->stop_flag && event_stop_flag;)
                {
                    if (ExecDispatch(function, mode))
                    {
                        continue;
                    }
                }

                EVENT_INFO("cyclic event stop running this: %p", this);

                if (0 == event_stop_flag.load(std::memory_order_acquire))
                {
                    return ChangeStatus(Status::Exec, Status::Stop);
                }
            }
        }
        return false;
    }

    bool EventLoop::StopDispatch()
    {
        if (nullptr != event_base)
        {
            if (EventThread() && ExistsRuning())
            {
                while (event_watcher->DestroyQueue());

                uv_stop(event_base);

                return ChangeStatus(Status::Exec, Status::Stop);
            }

            return RunInLoopEx(std::bind(&EventLoop::StopDispatch, this));
        }
        return false;
    }

    bool EventLoop::StopDispatchEx()
    {
        if (nullptr != event_base)
        {
            if (EventThread() && ExistsRuning())
            {
                while (event_watcher->DestroyQueue());

                uv_stop(event_base);

                if (event_stop_flag)
                {
                    event_stop_flag.store(0, std::memory_order_release);
                    event_stop_flag.notify_one();
                }

                return true;
            }

            if (RunInLoopEx(std::bind(&EventLoop::StopDispatchEx, this)))
            {
                event_stop_flag.wait(1, std::memory_order_relaxed);
            }
            return true;
        }
        return false;
    }

    bool EventLoop::RunInLoop(const Functor& function)
    {
        if (nullptr != event_watcher)
        {
            if (ExistsRuning())
            {
                return event_watcher->RunInLoop(function);
            }
        }
        return false;
    }

    bool EventLoop::RunInLoop(Functor&& function)
    {
        if (nullptr != event_watcher)
        {
            if (ExistsRuning())
            {
                return event_watcher->RunInLoop(std::move(function));
            }
        }
        return false;
    }

    bool EventLoop::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_watcher)
        {
            if (ExistsRuning())
            {
                return event_watcher->RunInLoopEx(function);
            }
        }
        return false;
    }

    bool EventLoop::RunInLoopEx(Handler&& function)
    {
        if (nullptr != event_watcher)
        {
            if (ExistsRuning())
            {
                return event_watcher->RunInLoopEx(std::move(function));
            }
        }
        return false;
    }

    bool EventLoop::AssignTimer(const u96 index, const u64 delay, const u64 repeat)
    {
        if (nullptr != event_timer_pool)
        {
            if (ExistsRuning())
            {
                return event_timer_pool->AssignTimer(index, delay, repeat);
            }
        }
        return false;
    }

    bool EventLoop::StopedTimer(const u96 index)
    {
        if (nullptr != event_timer_pool)
        {
            if (ExistsRuning())
            {
                return event_timer_pool->StopedTimer(index);
            }
        }
        return false;
    }

    void EventLoop::KilledTimer(const u96 index)
    {
        if (nullptr != event_timer_pool)
        {
            if (ExistsRuning())
            {
                return event_timer_pool->KilledTimer(index);
            }
        }
    }

    void EventLoop::ModiyRepeat(const u96 index, const u64 repeat)
    {
        if (nullptr != event_timer_pool)
        {
            if (ExistsRuning())
            {
                return event_timer_pool->ModiyRepeat(index, repeat);
            }
        }
        return;
    }

    bool EventLoop::ReStarTimer(const u96 index)
    {
        if (nullptr != event_timer_pool)
        {
            if (ExistsRuning())
            {
                return event_timer_pool->ReStarTimer(index);
            }
        }
        return false;
    }

    bool EventLoop::ReStarTimerEx(const u96 index, const u64 delay, const u64 repeat)
    {
        if (nullptr != event_timer_pool)
        {
            if (ExistsRuning())
            {
                return event_timer_pool->ReStarTimerEx(index, delay, repeat);
            }
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

    EventLoop* EventLoop::AddRefer()
    {
        ++event_refer;
        return this;
    }
}