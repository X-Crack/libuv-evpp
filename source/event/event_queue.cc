#include <event_queue.h>
#include <event_loop.h>
#include <event_async.h>
#include <event_mutex.h>
#include <event_coroutine.h>
#include <concurrentqueue.h>
namespace Evpp
{
    class EventQueueTraits : public moodycamel::ConcurrentQueueDefaultTraits
    {
    public:
        static const size_t BLOCK_SIZE = 8192;
        static const size_t EXPLICIT_INITIAL_INDEX_SIZE = 1024;
        static const size_t IMPLICIT_INITIAL_INDEX_SIZE = 1024;
        static const std::uint32_t EXPLICIT_CONSUMER_CONSUMPTION_QUOTA_BEFORE_ROTATE = 4096;
    };

    EventQueue::EventQueue(EventLoop* base) :
        event_base(base),
        event_queue(std::make_unique<EventAsync>(base, std::bind(&EventQueue::EventNotify, this))),
        event_queue_ex(std::make_unique<EventAsync>(base, std::bind(&EventQueue::EventNotifyEx, this))),
        event_queue_mutex(std::make_unique<EventMutex>()),
        event_queue_nolock(std::make_unique<moodycamel::ConcurrentQueue<Handler, EventQueueTraits>>()),
        event_queue_lock(std::make_unique<moodycamel::ConcurrentQueue<Handler, EventQueueTraits>>())
    {

    }

    EventQueue::~EventQueue()
    {

    }

    bool EventQueue::CreaterQueue()
    {
        return event_queue->CreaterAsync() && event_queue_ex->CreaterAsync();
    }

    bool EventQueue::DestroyQueue()
    {
        if (event_base->EventThread())
        {
            if (event_queue->DestroyAsync() && event_queue_ex->DestroyAsync())
            {
                while (event_queue_nolock->size_approx()) { EventNotify(); };
                while (event_queue_lock->size_approx()) { EventNotifyEx(); };

                return true;
            }
            return false;
        }
        return RunInLoopEx(std::bind(&EventQueue::DestroyQueue, this));
    }

    bool EventQueue::RunInLoop(const Handler& function)
    {
        if (nullptr != event_base && nullptr != event_queue && nullptr != event_queue_nolock)
        {
            if (event_base->EventThread())
            {
                if (nullptr != function)
                {
                    function();
                }
                return true;
            }
            return EmplaceQueue(function);
        }
        return false;
    }

    bool EventQueue::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_base && nullptr != event_queue_ex && nullptr != event_queue_lock)
        {
            if (EmplaceQueueEx(function))
            {
                if (event_base->EventThread())
                {
                    return true;
                }
                return event_queue_mutex->lock();
            }
        }
        return false;
    }

    bool EventQueue::RunInQueue(const Handler& function)
    {
        if (nullptr != event_base && nullptr != event_queue && nullptr != event_queue_nolock)
        {
            return EmplaceQueue(function);
        }
        return false;
    }

    bool EventQueue::EmplaceQueue(const Handler& function)
    {
        if (nullptr != event_base && nullptr != event_queue && nullptr != event_queue_nolock)
        {
            while (0 == event_queue_nolock->try_enqueue(function));

            return event_queue->ExecNotify();
        }
        return false;
    }

    bool EventQueue::EmplaceQueueEx(const Handler& function)
    {
        if (nullptr != event_base && nullptr != event_queue_ex && nullptr != event_queue_lock)
        {
            while (0 == event_queue_lock->try_enqueue(function));

            return event_queue_ex->ExecNotify();
        }
        return false;
    }

    void EventQueue::EventNotify()
    {
        while (event_queue_nolock->try_dequeue(event_queue_nolock_function))
        {
            if (nullptr != event_queue_nolock_function)
            {
#if defined(EVPP_USE_STL_COROUTINES)
                try
                {
                    if (JoinInTask(std::bind(event_queue_nolock_function)).get())
                    {
                        continue;
                    }
                }
                catch (...)
                {
                    assert(0);
                }
#else
                event_queue_nolock_function();
#endif
            }
        }
    }

    void EventQueue::EventNotifyEx()
    {
        while (event_queue_lock->try_dequeue(event_queue_lock_function))
        {
            if (nullptr != event_queue_lock_function)
            {
#if defined(EVPP_USE_STL_COROUTINES)
                try
                {
                    if (JoinInTask(std::bind(event_queue_lock_function)).get())
                    {
                        continue;
                    }
                }
                catch (...)
                {
                    assert(0);
                }
#else
                event_queue_lock_function();
#endif
            }
        }

        if (event_base->EventThread())
        {
            if (event_queue_mutex->unlock())
            {
                return;
            }
        }
    }
}