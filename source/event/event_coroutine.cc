#include <event_coroutine.h>
namespace Evpp
{
#ifdef __cpp_coroutines
    EventCoroutine::EventCoroutine(EventCoroutineTask* middle) : task_data(middle)
    {

    }

    EventCoroutine::~EventCoroutine()
    {

    }

    EventCoroutine EventCoroutine::promise_type::get_return_object_on_allocation_failure()
    {
        return EventCoroutine(nullptr);
    }

    EventCoroutine EventCoroutine::promise_type::get_return_object()
    {
        return EventCoroutine(task_base.get());
    };

    std::experimental::suspend_always EventCoroutine::promise_type::initial_suspend()
    {
        if (nullptr == task_base->handler)
        {
            task_base->handler = std::experimental::coroutine_handle<promise_type>::from_promise(*this);
        }
        return std::experimental::suspend_always();
    }


    std::experimental::suspend_always EventCoroutine::promise_type::final_suspend()
    {
        return std::experimental::suspend_always();
    };

    void EventCoroutine::promise_type::unhandled_exception()
    {
        throw;
    }

    void EventCoroutine::promise_type::return_void()
    {
        if (nullptr != task_base && nullptr != task_base->handler)
        {
            if (task_base->reflock.load())
            {
                task_base->reflock.store(0);
                task_base->handler.destroy();
            }
        }
    }

    std::experimental::suspend_always EventCoroutine::promise_type::yield_value(EventCoroutineTask* data)
    {
        if (nullptr != task_base)
        {
            if (nullptr != data)
            {
                task_base->handler = std::experimental::coroutine_handle<promise_type>::from_promise(*this);
                task_base->function = data->function;
                task_base->reflock.store(1);
                data->function();
                data = task_base.get();
            }
        }
        return std::experimental::suspend_always();
    }

    bool EventCoroutine::AwaitReady() noexcept
    {
        if (nullptr != task_data && nullptr != task_data->handler)
        {
            return task_data->handler.done();
        }
        return false;
    }

    EventCoroutine EventCoroutine::JoinInTask()
    {
        return JoinInTask(task_data);
    }

    EventCoroutine EventCoroutine::JoinInTask(EventCoroutineTask* middle) noexcept
    {
        if (nullptr != middle && nullptr != middle->function)
        {
            co_yield middle;
        }
        co_return;
    }

    bool EventCoroutine::ResumeTask()
    {
        if (nullptr != task_data && nullptr != task_data->handler)
        {
            task_data->handler.resume();
            return true;
        }
        return false;
    }

    bool EventCoroutine::SubmitTask()
    {
        if (AwaitReady())
        {
            return true;
        }

        return ResumeTask();
    }

    bool EventCoroutine::CancelTask()
    {
        if (AwaitReady())
        {
            return true;
        }

        return ResumeTask();
    }

    bool EventCoroutine::SubmitTaskEx()
    {
        return SubmitTask() && CancelTask();
    }

    EventCoroutine& EventCoroutine::operator++()
    {
        if (nullptr != task_data && nullptr != task_data->handler)
        {
            if (task_data->handler.done())
            {
                task_data->handler = nullptr;
            }
            else
            {
                task_data->handler.resume();
            }
        }
        return *this;
    }

    bool EventCoroutine::operator==(const EventCoroutine& rhs)
    {
        return task_data == rhs.task_data;
    }

    bool EventCoroutine::operator!=(const EventCoroutine& rhs)
    {
        return !(*this == rhs);
    }

    bool EventCoroutine::operator==(EventCoroutine* rhs)
    {
        return task_data == rhs->task_data;
    }

    bool EventCoroutine::operator!=(EventCoroutine* rhs)
    {
        return !(this == rhs);
    }

    EventCoroutineTask* EventCoroutine::operator->()
    {
        if (nullptr != task_data && nullptr != task_data->handler)
        {
            return task_data->handler.promise().task_base.get();
        }
        return nullptr;
    }

    EventCoroutineTask* EventCoroutine::operator*()
    {
        return operator->();
    }

    bool EventCoroutineTask::DestroyTask()
    {
        if (nullptr != handler)
        {
            handler.destroy();
            return true;
        }
        return false;
    }

    EventCoroutineTask* EventCoroutineTask::GetHandlerInstance()
    {
        return static_cast<EventCoroutineTask*>(handler.address());
    }
#endif
}