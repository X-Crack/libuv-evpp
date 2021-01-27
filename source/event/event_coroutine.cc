#include <event_coroutine.h>
namespace Evpp
{
#ifdef __cpp_coroutines
    EventCoroutine::EventCoroutine(EventCoroutineMiddle* middle) : middle_data(middle)
    {

    }

    EventCoroutine::~EventCoroutine()
    {
        if (nullptr != middle_data && nullptr != middle_data->handler)
        {
            middle_data->handler.destroy();
        }
    }

    EventCoroutine EventCoroutine::promise_type::get_return_object_on_allocation_failure()
    {
        return EventCoroutine(nullptr);
    }

    EventCoroutine EventCoroutine::promise_type::get_return_object()
    {
        return EventCoroutine(middle_refer.get());
    };

    std::experimental::suspend_always EventCoroutine::promise_type::initial_suspend()
    {
        if (nullptr == middle_refer->handler)
        {
            middle_refer->handler = std::experimental::coroutine_handle<promise_type>::from_promise(*this);
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
        if (nullptr != middle_refer && nullptr != middle_refer->handler)
        {
            if (middle_refer->reflock.load(std::memory_order_release))
            {
                middle_refer->reflock.store(0);
                middle_refer->handler = nullptr;
            }
        }
    }

    std::experimental::suspend_always EventCoroutine::promise_type::yield_value(EventCoroutineMiddle* data)
    {
        if (nullptr != middle_refer)
        {
            data->function();
            middle_refer->handler = std::experimental::coroutine_handle<promise_type>::from_promise(*this);
            middle_refer->reflock.store(1);
            data = middle_refer.get();
        }
        return std::experimental::suspend_always();
    }

    bool EventCoroutine::AwaitReady() noexcept
    {
        if (nullptr != middle_data && nullptr != middle_data->handler)
        {
            return middle_data->handler.done();
        }
        return false;
    }

    bool EventCoroutine::GetTaskSuccess()
    {
        if (middle_data && middle_data->handler)
        {
            return 1 == middle_data->reflock;
        }
        return true;
    }

    EventCoroutine EventCoroutine::JoinInTask()
    {
        return JoinInTask(middle_data);
    }

    EventCoroutine EventCoroutine::JoinInTask(EventCoroutineMiddle* middle) noexcept
    {
        if (nullptr != middle && nullptr != middle->function)
        {
            co_yield middle;
        }
        co_return;
    }

    bool EventCoroutine::SubmitTask()
    {
        if (AwaitReady())
        {
            return true;
        }

        if (nullptr != middle_data && nullptr != middle_data->handler)
        {
            middle_data->handler.resume();
            return true;
        }
        return false;
    }

    bool EventCoroutine::CancelTask()
    {
        if (AwaitReady())
        {
            return true;
        }

        if (nullptr != middle_data && nullptr != middle_data->handler)
        {
            middle_data->handler.resume();
            return true;
        }
        return false;
    }

    bool EventCoroutine::SubmitTaskEx()
    {
        return SubmitTask() && CancelTask();
    }

    EventCoroutine& EventCoroutine::operator++()
    {
        if (nullptr != middle_data && nullptr != middle_data->handler)
        {
            if (middle_data->handler.done())
            {
                middle_data->handler = nullptr;
            }
            else
            {
                middle_data->handler.resume();
            }
        }
        return *this;
    }

    bool EventCoroutine::operator==(const EventCoroutine& rhs)
    {
        return middle_data == rhs.middle_data;
    }

    bool EventCoroutine::operator!=(const EventCoroutine& rhs)
    {
        return !(*this == rhs);
    }

    bool EventCoroutine::operator==(EventCoroutine* rhs)
    {
        return middle_data == rhs->middle_data;
    }

    bool EventCoroutine::operator!=(EventCoroutine* rhs)
    {
        return !(this == rhs);
    }

    EventCoroutineMiddle* EventCoroutine::operator->()
    {
        if (nullptr != middle_data && nullptr != middle_data->handler)
        {
            return middle_data->handler.promise().middle_refer.get();
        }
        return nullptr;
    }

    EventCoroutineMiddle* EventCoroutine::operator*()
    {
        return operator->();
    }

    bool EventCoroutineMiddle::DestroyTask()
    {
        if (nullptr != handler)
        {
            if (StoptheTask())
            {
                handler.destroy();
                return true;
            }
        }
        return false;
    }

    EventCoroutineMiddle* EventCoroutineMiddle::GetHandlerInstance()
    {
        return static_cast<EventCoroutineMiddle*>(handler.address());
    }

    bool EventCoroutineMiddle::StoptheTask()
    {
        return 0 == reflock.load(std::memory_order_release);
    }
#endif
}