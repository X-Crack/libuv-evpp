#include <event_loop.h>
#include <event_watcher.h>
#include <event_timer_pool.h>
#include <event_mutex.h>
namespace Evpp
{
    EventLoop::EventLoop(event_loop* loop, const u96 index) :
        event_base(loop),
        event_index(index),
        event_refer(0),
        event_watcher(std::make_unique<EventWatcher>(this)),
        event_timer_pool(std::make_unique<EventTimerPool>(this)),
        event_thread(0),
        event_stop_flag(1),
        event_stop_flag_ex(1)
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

    bool EventLoop::ExecDispatch(i32 mode)
    {
        if (nullptr != event_base)
        {
            if (ExecDispatchEx(mode))
            {
                return SwitchDispatch();
            }
            return true;
        }
        return false;
    }

    bool EventLoop::ExecDispatch(const EventLoopHandler& function, i32 mode)
    {
        if (nullptr != event_base)
        {
            if (ExecDispatchEx(mode))
            {
                if (nullptr != function)
                {
                    function(this);
                }
            }
            return SwitchDispatch();
        }
        return false;
    }

    bool EventLoop::ExecLoopDispatch(const EventLoopHandler& function, i32 mode)
    {
        if (nullptr != event_base)
        {
            if (ChangeStatus(Status::Init, Status::Exec))
            {
                for (; 0 == SwitchDispatch();)
                {
                    if (ExecDispatchEx(mode))
                    {
                        if (nullptr != function)
                        {
                            function(this);
                        }
                    }
                }

                EVENT_INFO("cyclic event stop running this: %p", this);
                return true;
            }
            assert(0);
        }
        return false;
    }

    bool EventLoop::StopDispatch()
    {
        if (nullptr != event_base)
        {
            if (ExistsRuning())
            {
                if (EventLoopAlive(event_base) && 1 == event_stop_flag.load(std::memory_order_acquire))
                {
                    if (RunInLoopEx(std::bind(&EventWatcher::DestroyAsync, event_watcher.get())))
                    {
                        event_stop_flag.store(0, std::memory_order_release);
                        event_stop_flag.wait(0, std::memory_order_relaxed);
                        return true;
                    }
                }
                return ChangeStatus(Status::Stop, Status::Exit);
            }
        }
        return false;
    }

    bool EventLoop::StopDispatchEx()
    {
        if (nullptr != event_base)
        {
            if (EventThread() && ExistsRuning())
            {
                if (0 == EventLoopAlive(event_base))
                {
                    return ChangeStatus(Status::Exec, Status::Stop);
                }

                if (event_watcher->DestroyAsync())
                {
                    while (event_watcher->DestroyQueue()) { EVPP_THREAD_YIELD(); };

                    if (0 == event_base->stop_flag)
                    {
                        uv_stop(event_base);
                    }

                    if (event_stop_flag_ex)
                    {
                        event_stop_flag_ex.store(0, std::memory_order_release);
                        event_stop_flag_ex.notify_one();
                    }

                    return ChangeStatus(Status::Exec, Status::Stop);
                }
            }

            if (RunInLoopEx(std::bind(&EventLoop::StopDispatchEx, this)))
            {
                event_stop_flag_ex.wait(1, std::memory_order_relaxed);
            }
            return true;
        }
        return false;
    }

    bool EventLoop::RunInLoop(const Functor& function)
    {
        if (nullptr != event_watcher)
        {
            if (ExistsRuning() || ExistsInited())
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
            if (ExistsRuning() || ExistsInited())
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
            if (ExistsRuning() || ExistsInited())
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
            if (ExistsRuning() || ExistsInited())
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
            if (ExistsRuning() || ExistsInited())
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
            if (ExistsRuning() || ExistsInited())
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
            if (ExistsRuning() || ExistsInited())
            {
                return event_timer_pool->KilledTimer(index);
            }
        }
    }

    void EventLoop::ModiyRepeat(const u96 index, const u64 repeat)
    {
        if (nullptr != event_timer_pool)
        {
            if (ExistsRuning() || ExistsInited())
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
            if (ExistsRuning() || ExistsInited())
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
            if (ExistsRuning() || ExistsInited())
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

    event_loop* EventLoop::EventBasic()
    {
        return event_base;
    }

    EventLoop* EventLoop::AddRefer()
    {
        ++event_refer;
        return this;
    }

    bool EventLoop::ExecDispatchEx(i32 mode)
    {
        if (nullptr != event_base)
        {
            if (ExistsInited())
            {
                ChangeStatus(Status::Exec);
            }

            return (UV_RUN_DEFAULT == mode ? 0 : UV_RUN_ONCE || UV_RUN_NOWAIT == mode ? 1 : 0) == uv_run(event_base, static_cast<uv_run_mode>(mode));
        }
        return false;
    }

    bool EventLoop::SwitchDispatch()
    {
        if (EventLoopAlive(event_base))
        {
            return false;
        }

        if (event_stop_flag.load(std::memory_order_acquire))
        {
            return false;
        }

        if (event_stop_flag.exchange(1, std::memory_order_release))
        {
            return false;
        }

        if (0 != event_base->active_handles || 0 != uv_loop_close(event_base))
        {
            assert(0);
            return false;
        }

        if (ChangeStatus(Status::Exec, Status::Stop))
        {
            event_stop_flag.notify_one();
            return true;
        }
        return false;
    }
}
