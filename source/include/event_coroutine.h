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
    struct EventCoroutine
    {
    public:
        struct promise_type;
        using handle_type = std::experimental::coroutine_handle<promise_type>;
    public:
        explicit EventCoroutine(handle_type handler_t) :handle(handler_t) { }
        virtual ~EventCoroutine()
        {
            if (nullptr != handle)
            {
                handle.destroy();
            }
        }
        bool get()
        {
            if (!(this->handle.done()))
            {
                handle.resume();
                {
                    return handle.promise().value;
                }

            }
            return 0;
        }
    public:
        struct promise_type
        {
        public:
            explicit promise_type() : value(false) {}
        public:
            friend EventCoroutine;
        public:
            auto get_return_object()
            {
                return EventCoroutine{ handle_type::from_promise(*this) };
            }

            auto initial_suspend()
            {
                return std::experimental::suspend_always{};
            }

            auto return_value(bool var)
            {
                value = var;
                return std::experimental::suspend_always{};
            }

            auto yield_value(bool var)
            {
                value = var;
                return std::experimental::suspend_always{};
            }

            auto final_suspend()
            {
                return std::experimental::suspend_always{};
            }

            void unhandled_exception()
            {
                throw;
            }
        private:
            bool value;
        };
    private:
        handle_type handle;
    };

    struct EventCoroutineTask
    {
    public:
        explicit EventCoroutineTask(std::function<void()> callback) : function(callback) { }
    public:
        bool await_ready()
        {
            return true;
        }

        auto await_suspend(std::experimental::coroutine_handle<> awaiting_handle)
        {
            return false;
        }

        auto await_resume()
        {
            return function();
        }
    private:
        std::function<void()>                                            function;
    };

    struct EventCoroutineTaskEx
    {
    public:
        explicit EventCoroutineTaskEx(std::function<bool()> callback) : function(callback) { }
    public:
        bool await_ready()
        {
            return true;
        }

        auto await_suspend(std::experimental::coroutine_handle<> awaiting_handle)
        {
            return false;
        }

        auto await_resume()
        {
            return function();
        }
    private:
        std::function<bool()>                                            function;
    };

    EventCoroutine JoinInTask(std::function<void()> callback);
    EventCoroutine JoinInTaskEx(std::function<bool()> callback);
#endif
}
#endif // __EVENT_COROUTINE_H__
