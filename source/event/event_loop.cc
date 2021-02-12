#include <event_loop.h>
#include <event_queue.h>
#include <event_idle.h>
#include <event_timer_pool.h>
#include <event_mutex.h>
#include <event_coroutine.h>
namespace Evpp
{
    EventLoop::EventLoop(event_loop* loop, const u96 index) :
        event_base(loop),
        event_index(index),
        event_refer(0),
        event_queue(std::make_unique<EventQueue>(this)),
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
                if (0 == event_thread)
                {
                    event_thread = EventThreadId();
                }

                if (nullptr == event_base->data)
                {
                    event_base->data = this;

                }
                return event_queue->CreaterQueue();
            }

            EVENT_INFO("an error occurred during the initialization loop this: %p", this);
        }
        return false;
    }

    bool EventLoop::ExecDispatch(i32 mode)
    {
        if (nullptr != event_base)
        {
            if (ExecDispatchEvent(mode))
            {
                return SwitchDispatch();
            }
        }
        return false;
    }

    bool EventLoop::ExecDispatch(const EventLoopHandler& function, i32 mode)
    {
        if (nullptr != event_base)
        {
            if (ExecDispatchEvent(mode))
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

    bool EventLoop::ExecDispatchEx(const EventLoopHandler& function, i32 mode)
    {
        if (nullptr != event_base)
        {
            for (; 0 == SwitchDispatch();)
            {
#if defined(EVPP_USE_STL_COROUTINES)
                if (JoinInTaskEx(std::bind(&EventLoop::ExecDispatchEvent, this, mode)).get())
                {
                    EVENT_INFO("cyclic event stop running this: %p", this);
                    continue; // uv_run 的生命结束 因为没有handlers存活
                }
#else
                if (ExecDispatchEvent(mode))
                {
                    continue;
                }
#endif
                if (nullptr != function)
                {
                    function(this);
                }
            }
            return true;
        }
        return false;
    }

    bool EventLoop::StopDispatch()
    {
        EVENT_COMPUTE_DURATION(StopDispatchEx);
        if (nullptr != event_base)
        {
            if (EventThread() && ExistsRuning())
            {
                if (0 == EventLoopAlive(event_base))
                {
                    return ChangeStatus(Status::Stop, Status::Exit);
                }

                if (event_queue->DestroyQueue())
                {
                    return ChangeStatus(Status::Stop, Status::Exit);
                }
                return false;
            }

            if (RunInLoopEx(std::bind(&EventLoop::StopDispatch, this)))
            {
                return event_mutex.lock();
            }
        }
        return false;
    }

    bool EventLoop::RunInLoop(const Handler& function)
    {
        if (nullptr != event_queue)
        {
            if (ExistsRuning() || ExistsInited())
            {
                return event_queue->RunInLoop(function);
            }
        }
        return false;
    }

    bool EventLoop::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_queue)
        {
            if (ExistsRuning() || ExistsInited())
            {
                return event_queue->RunInLoopEx(function);
            }
        }
        return false;
    }

    bool EventLoop::RunInQueue(const Handler& function)
    {
        if (nullptr != event_queue)
        {
            if (ExistsRuning() || ExistsInited())
            {
                return event_queue->RunInQueue(function);
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

    bool EventLoop::KilledTimer(const u96 index)
    {
        if (nullptr != event_timer_pool)
        {
            if (ExistsRuning() || ExistsInited())
            {
                return event_timer_pool->KilledTimer(index);
            }
        }
        return false;
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

    bool EventLoop::ExecDispatchEvent(i32 mode)
    {
        if (nullptr != event_base)
        {
            if (ExistsInited())
            {
                ChangeStatus(Status::Exec);
            }

            if (ExistsStoped() || ExistsExited())
            {
                return true;
            }

            if (EventLoopAlive(event_base))
            {
                return 0 == uv_run(event_base, static_cast<uv_run_mode>(mode));
            }
            return true;
        }
        return false;
    }

    bool EventLoop::SwitchDispatch()
    {
        if (ExistsStoped() || ExistsExited())
        {
            return true;
        }

        if (EventLoopAlive(event_base))
        {
            return false;
        }

        if (ChangeStatus(Status::Exec, Status::Stop))
        {
            if (0 == event_base->stop_flag || ~0 == event_base->stop_flag)
            {
                uv_stop(event_base);
            }

            if (nullptr != event_base->data)
            {
                event_base->data = nullptr;
            }

            if (0 == uv_loop_close(event_base))
            {
                return event_mutex.unlock();
            }
            else
            {
                uv_walk(event_base, &EventLoop::ObserveHandler, 0);
            }
            return true;
        }

        return false;
    }

    void EventLoop::ObserveHandler(event_handle* handler, void* arg)
    {
        if (nullptr != handler)
        {
            EVENT_INFO("%s not closed check your code", uv_handle_type_name(handler->type));
            assert(0);
        }
    }
}
