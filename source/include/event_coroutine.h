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
#if defined(EVPP_USE_STL_COROUTINES)
    struct EventCoroutineTask;
    struct EventCoroutine
    {
        struct promise_type;
        using handle_type = std::experimental::coroutine_handle<promise_type>;
        struct promise_type
        {
            auto get_return_object() { return EventCoroutine{}; };
            auto initial_suspend() { return std::experimental::suspend_never{}; };
            auto final_suspend() { return std::experimental::suspend_never{}; };
            void unhandled_exception() { throw; };
            auto yield_value(const bool var) { return std::experimental::suspend_always{}; };
            void return_void() { };
        };

        static EventCoroutine JoinInTask(const std::function<void()>& callback);
        static EventCoroutine JoinInTaskEx(const std::function<bool()>& callback);
    };

    struct EventCoroutineTask
    {
    public:
        explicit EventCoroutineTask() : function(nullptr) {};
        explicit EventCoroutineTask(std::function<void()> callback) : function(callback) {};
    public:
        bool await_ready() const
        {
            return false;
        }

        bool await_resume()
        {
            return true;
        }

        void await_suspend(std::experimental::coroutine_handle<> handle)
        {
            if (nullptr != function)
            {
                function();
            }
            handle.resume();
        }
    public:
        std::function<void()>                                                                       function;
    };

    struct EventCoroutineTaskEx
    {
    public:
        explicit EventCoroutineTaskEx() : function(nullptr), result(false) {};
        explicit EventCoroutineTaskEx(std::function<bool()> callback) : function(callback), result(false) {};
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
#endif
}
#endif // __EVENT_COROUTINE_H__
