#ifndef __EVENT_COROUTINE_H__
#define __EVENT_COROUTINE_H__
#include <config.h>
#include <memory>
#include <atomic>
#include <functional>
#include <experimental/coroutine>
#include <experimental/resumable>
namespace Evpp
{
#ifdef __cpp_coroutines
    class EventCoroutineMiddle;
    class EventCoroutine final
    {
    public:
        explicit EventCoroutine(EventCoroutineMiddle* middle = nullptr);
        virtual ~EventCoroutine();
    public:
        bool AwaitReady() noexcept;
    public:
        bool GetTaskSuccess();
        EventCoroutine JoinInTask();
        EventCoroutine JoinInTask(EventCoroutineMiddle* Middle) noexcept;
        bool SubmitTask();
        bool CancelTask();
        bool SubmitTaskEx();
    private:
        EventCoroutine& operator++(i32) = delete;
        EventCoroutine& operator++();
        bool operator==(const EventCoroutine& rhs);
        bool operator!=(const EventCoroutine& rhs);
        bool operator==(EventCoroutine* rhs);
        bool operator!=(EventCoroutine* rhs);
        EventCoroutineMiddle* operator->();
        EventCoroutineMiddle* operator*();
    public:
        class promise_type
        {
        public:
            explicit promise_type() : middle_refer(std::make_unique<EventCoroutineMiddle>()) { printf("middle_refer:%p\n", middle_refer.get()); }
        public:
            static EventCoroutine get_return_object_on_allocation_failure();
            EventCoroutine get_return_object();
            std::experimental::suspend_always initial_suspend();
            std::experimental::suspend_always final_suspend();
            void unhandled_exception();
            void return_void();
            std::experimental::suspend_always yield_value(EventCoroutineMiddle* data);
        public:
            std::unique_ptr<EventCoroutineMiddle>                                                   middle_refer;
        };
    private:
        EventCoroutineMiddle*                                                                       middle_data;
        std::atomic<u32>                                                                            exit_loop;
    };

    class EventCoroutineMiddle
    {
    public:
        explicit EventCoroutineMiddle() : function(nullptr), reflock(0) {};
        explicit EventCoroutineMiddle(const std::function<void()>& callback) : function(callback), reflock(0) {};
    public:
        bool DestroyTask();
        EventCoroutineMiddle* GetHandlerInstance();
        bool StoptheTask();
    private:
        virtual void EventCoroutineMiddleCallback() { return; };
    public:
        std::function<void()>                                                                       function;
        std::atomic<u32>                                                                            reflock;
        std::experimental::coroutine_handle<EventCoroutine::promise_type>                           handler;
    };
#endif
}
#endif // __EVENT_COROUTINE_H__