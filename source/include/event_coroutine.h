#ifndef __EVENT_COROUTINE_H__
#define __EVENT_COROUTINE_H__
#include <event_config.h>
#include <memory>
#include <atomic>
#include <functional>
#include <experimental/coroutine>
#include <experimental/resumable>
namespace Evpp
{
    struct EventCoroutineTask;
    struct EventCoroutine
    {
        struct promise_type
        {
            auto get_return_object() { return EventCoroutine{}; };
            auto initial_suspend() { return std::experimental::suspend_never{}; };
            auto final_suspend() { return std::experimental::suspend_never{}; };
            void unhandled_exception() { throw; };
            void return_void() { };
        };
        static EventCoroutine JoinInTask(const std::function<bool()>& callback);
    };

    struct EventCoroutineTask
    {
    public:
        explicit EventCoroutineTask() : function(nullptr), result(false) {};
        explicit EventCoroutineTask(std::function<bool()> callback) : function(callback), result(false) {};
    public:
        bool await_ready() const
        {
            return false;
        }

        bool await_resume()
        {
            return result;
        }

        void await_suspend(std::experimental::coroutine_handle<> handle)
        {
            if (nullptr != function)
            {
                result = function();
            }
            else
            {
                result = false;
            }
            handle.resume();
        }
    public:
        std::function<bool()>                                                                       function;
        bool                                                                                        result;
    };
}
#endif // __EVENT_COROUTINE_H__