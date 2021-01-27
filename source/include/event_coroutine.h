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
    class EventCoroutineTask;
    class EventCoroutine final
    {
    public:
        explicit EventCoroutine(EventCoroutineTask* task = nullptr);
        virtual ~EventCoroutine();
    public:
        bool AwaitReady() noexcept;
        EventCoroutine JoinInTask();
        static EventCoroutine JoinInTask(EventCoroutineTask* task) noexcept;
        bool ResumeTask();
        bool SubmitTask();
        bool CancelTask();
        bool SubmitTaskEx();
        EventCoroutineTask* Task() { return task_data; }
    private:
        EventCoroutine& operator++(i32) = delete;
        EventCoroutine& operator++();
        bool operator==(const EventCoroutine& rhs);
        bool operator!=(const EventCoroutine& rhs);
        bool operator==(EventCoroutine* rhs);
        bool operator!=(EventCoroutine* rhs);
        EventCoroutineTask* operator->();
        EventCoroutineTask* operator*();
    public:
        class promise_type
        {
        public:
            explicit promise_type() : task_base(std::make_unique<EventCoroutineTask>()) { }
        public:
            static EventCoroutine get_return_object_on_allocation_failure();
            EventCoroutine get_return_object();
            std::experimental::suspend_always initial_suspend();
            std::experimental::suspend_always final_suspend();
            void unhandled_exception();
            void return_void();
            std::experimental::suspend_always yield_value(EventCoroutineTask* data);
        public:
            std::unique_ptr<EventCoroutineTask>                                                     task_base;
        };
    private:
        EventCoroutineTask*                                                                         task_data;
    };

    class EventCoroutineTask
    {
    public:
        explicit EventCoroutineTask() : function(nullptr), reflock(0) {};
        explicit EventCoroutineTask(std::function<void()> callback) : function(callback), reflock(0) {};
    public:
        bool DestroyTask();
        EventCoroutineTask* GetHandlerInstance();
    protected:
        virtual void EventCoroutineTaskCallback() { return; };
    public:
        std::function<void()>                                                                       function;
        std::atomic<u32>                                                                            reflock;
        std::experimental::coroutine_handle<EventCoroutine::promise_type>                           handler;
    };
#endif
}
#endif // __EVENT_COROUTINE_H__