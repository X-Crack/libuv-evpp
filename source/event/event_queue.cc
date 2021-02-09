#include <event_queue.h>
#include <event_loop.h>
#include <event_async.h>
#include <event_mutex.h>
#include <event_coroutine.h>
#include <concurrentqueue.h>
namespace Evpp
{
#ifdef EVPP_USE_CAMERON314_CONCURRENTQUEUE
    class EventQueueTraits : public moodycamel::ConcurrentQueueDefaultTraits
    {
    public:
        static const size_t BLOCK_SIZE = 8192;
        static const size_t EXPLICIT_INITIAL_INDEX_SIZE = 1024;
        static const size_t IMPLICIT_INITIAL_INDEX_SIZE = 1024;
        static const std::uint32_t EXPLICIT_CONSUMER_CONSUMPTION_QUOTA_BEFORE_ROTATE = 4096;
    };
#endif
    EventQueue::EventQueue(EventLoop* base)
        : event_base(base)
        , event_queue(std::make_unique<EventAsync>(base, std::bind(&EventQueue::EventNotify, this)))
        , event_queue_ex(std::make_unique<EventAsync>(base, std::bind(&EventQueue::EventNotifyEx, this)))
        , event_queue_mutex(std::make_unique<EventMutex>())
#ifdef EVPP_USE_CAMERON314_CONCURRENTQUEUE
        , event_queue_nolock(std::make_unique<moodycamel::ConcurrentQueue<Handler, EventQueueTraits>>())
        , event_queue_lock(std::make_unique<moodycamel::ConcurrentQueue<Handler, EventQueueTraits>>())
#endif
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
#ifdef EVPP_USE_CAMERON314_CONCURRENTQUEUE
                while (event_queue_nolock->size_approx()) { EventNotify(); };
                while (event_queue_lock->size_approx()) { EventNotifyEx(); };
#else
                if (event_queue_nolock.size())
                {
                    EventNotify();
                }

                if (event_queue_lock.size())
                {
                    EventNotifyEx();
                }
#endif
                return true;
            }
            return false;
        }
        return RunInQueue(std::bind(&EventQueue::DestroyQueue, this));
    }

    bool EventQueue::RunInLoop(const Handler& function)
    {
        if (nullptr != event_base && nullptr != event_queue)
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
        if (nullptr != event_base && nullptr != event_queue_ex)
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
        if (nullptr != event_base && nullptr != event_queue)
        {
            return EmplaceQueue(function);
        }
        return false;
    }

    bool EventQueue::EmplaceQueue(const Handler& function)
    {
        if (nullptr != event_base && nullptr != event_queue)
        {
#if defined(EVPP_USE_CAMERON314_CONCURRENTQUEUE)
            while (0 == event_queue_nolock->try_enqueue(function));
#else
            {
                std::lock_guard<std::mutex> lock(event_queue_nolock_mutex);
                event_queue_nolock.emplace_back(function);
            }
#endif
            return event_queue->ExecNotify();
        }
        return false;
    }

    bool EventQueue::EmplaceQueueEx(const Handler& function)
    {
        if (nullptr != event_base && nullptr != event_queue_ex)
        {
#if defined(EVPP_USE_CAMERON314_CONCURRENTQUEUE)
            while (0 == event_queue_lock->try_enqueue(function));
#else
            {
                std::lock_guard<std::mutex> lock(event_queue_lock_mutex);
                event_queue_lock.emplace_back(function);
            }
#endif

            return event_queue_ex->ExecNotify();
        }
        return false;
    }

    void EventQueue::EventNotify()
    {
#if defined(EVPP_USE_CAMERON314_CONCURRENTQUEUE)
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
#else
        std::lock_guard<std::mutex> lock(event_queue_nolock_mutex);
        {
            std::vector<Handler> functors;
            {
                functors.swap(event_queue_nolock);
            }

            for (const auto& function : functors)
            {
#if defined(EVPP_USE_STL_COROUTINES)
                try
                {
                    if (JoinInTask(std::bind(function)).get())
                    {
                        continue;
                    }
                }
                catch (...)
                {
                    assert(0);
                }
#else
                function();
#endif
            }
        }
#endif
    }

    void EventQueue::EventNotifyEx()
    {
#if defined(EVPP_USE_CAMERON314_CONCURRENTQUEUE)
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
#else
        std::lock_guard<std::mutex> lock(event_queue_lock_mutex);
        {
            std::vector<Handler> functors;
            {
                functors.swap(event_queue_lock);
            }

            for (const auto& function : functors)
            {
#if defined(EVPP_USE_STL_COROUTINES)
                try
                {
                    if (JoinInTask(std::bind(function)).get())
                    {
                        continue;
                    }
                }
                catch (...)
                {
                    assert(0);
                }
#else
                function();
#endif
            }
        }
#endif
        if (event_base->EventThread())
        {
            if (event_queue_mutex->unlock())
            {
                return;
            }
        }
    }
}