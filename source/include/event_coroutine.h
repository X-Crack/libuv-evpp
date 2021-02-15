#ifndef __EVENT_COROUTINE_H__
#define __EVENT_COROUTINE_H__
#include <event_config.h>
#include <memory>
#include <atomic>
#include <functional>
#include <event_exception.h>
#if defined(EVPP_USE_STL_COROUTINES)
#include <experimental/coroutine>
#include <experimental/resumable>
#endif
namespace Evpp
{
#if defined(EVPP_USE_STL_COROUTINES)
    struct EVPP_EXPORT EventCoroutine
    {
    public:
        struct promise_type;
        using handle_type = std::experimental::coroutine_handle<promise_type>;
    public:
        explicit EventCoroutine(handle_type handler_t) :handler(handler_t) { }
        virtual ~EventCoroutine()
        {
            if (nullptr != handler)
            {
                handler.destroy();
            }
        }
        bool get()
        {
            if (0 == this->handler.done())
            {
                handler.resume();
                {
                    return handler.promise().value;
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

            void return_value(bool var)
            {
                value = var;
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
                throw EventException("unhandled_exception");
            }
        private:
            bool                                                                        value;
        };
    private:
        handle_type                                                                     handler;
    };

    struct EVPP_EXPORT EventCoroutineTask
    {
    public:
        explicit EventCoroutineTask(Handler&& callback) : function(std::move(callback)) { }
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
        Handler                                                         function;
    };

    struct EVPP_EXPORT EventCoroutineTaskEx
    {
    public:
        explicit EventCoroutineTaskEx(Functor&& callback) : function(std::move(callback)) { }
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
        Functor                                                         function;
    };

    EventCoroutine EVPP_EXPORT JoinInTask(Handler&& callback);
    EventCoroutine EVPP_EXPORT JoinInTaskEx(Functor&& callback);
#endif
}
#endif // __EVENT_COROUTINE_H__
