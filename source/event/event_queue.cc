#include <event_queue.h>
#include <event_loop.h>
#include <event_async.h>
#include <event_coroutine.h>
#include <event_exception.h>
#include <event_mutex.h>
#if defined(EVPP_USE_CAMERON314_CONCURRENTQUEUE)
#include <concurrentqueue.h>
#include <blockingconcurrentqueue.h>
#elif defined(EVPP_USE_BOOST_LOCKFREE_QUEUE)
#include <boost/lockfree/queue.hpp>
#endif
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
#if defined(EVPP_USE_CAMERON314_CONCURRENTQUEUE)
        , event_queue_nolock(std::make_unique<moodycamel::ConcurrentQueue<Handler, EventQueueTraits>>())
        , event_queue_lock(std::make_unique<moodycamel::BlockingConcurrentQueue<Handler, EventQueueTraits>>())
#elif defined(EVPP_USE_BOOST_LOCKFREE_QUEUE)
        , event_queue_nolock(std::make_unique<boost::lockfree::queue<Handler*>>(1024))
        , event_queue_lock(std::make_unique<boost::lockfree::queue<Handler*>>(512))
        , event_queue_nolock_function(nullptr)
        , event_queue_lock_function(nullptr)
#endif
        , event_queue_nolock_function_count(0)
        , evebt_queue_lock_function_count(0)
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
#if defined(EVPP_USE_CAMERON314_CONCURRENTQUEUE)
                while (event_queue_nolock->size_approx())
                {
                    EventNotify(); 
                };
                while (event_queue_lock->size_approx()) 
                { 
                    EventNotifyEx(); 
                };
#elif defined(EVPP_USE_BOOST_LOCKFREE_QUEUE)
                while (event_queue_nolock_function_count.load(std::memory_order_acquire)) { EventNotify(); };
                while (evebt_queue_lock_function_count.load(std::memory_order_acquire)) { EventNotifyEx(); };
#else
                if (event_queue_nolock.size()) { EventNotify(); };
                if (event_queue_lock.size()) { EventNotifyEx(); };
#endif
                return true;
            }
            return false;
        }
        return RunInLoopEx(std::bind(&EventQueue::DestroyQueue, this));
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
                return true;
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
#elif defined(EVPP_USE_BOOST_LOCKFREE_QUEUE)
            event_queue_nolock_function_count.fetch_add(1, std::memory_order_release);
            while (0 == event_queue_nolock->push(new Handler(function)));
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
#elif defined(EVPP_USE_BOOST_LOCKFREE_QUEUE)
            evebt_queue_lock_function_count.fetch_add(1, std::memory_order_release);
            while (0 == event_queue_lock->push(new Handler(function)));
#else
            evebt_queue_lock_function_count.fetch_add(1, std::memory_order_release);
            {
                std::lock_guard<std::mutex> lock(event_queue_lock_mutex);
                event_queue_lock.emplace_back(function);
            }
#endif
            return event_queue_ex->ExecNotify() && event_locking.dowait();
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
                try
                {
#if defined(EVPP_USE_STL_COROUTINES)
                    JoinInTask(std::bind(event_queue_nolock_function)).get();
#else
                    event_queue_nolock_function();
#endif
                }
                catch (const EventException& ex)
                {
                    EVENT_INFO("%s", ex.what());
                }
            }
        }
#elif defined(EVPP_USE_BOOST_LOCKFREE_QUEUE)
        while (event_queue_nolock->pop(event_queue_nolock_function))
        {
            if (nullptr != event_queue_nolock_function && nullptr != *event_queue_nolock_function)
            {
                try
                {
                    if (event_queue_nolock_function_count.fetch_sub(1, std::memory_order_relaxed))
                    {
                        SafeReleaseHandler destroy_object(std::addressof(event_queue_nolock_function));
                        {
#if defined(EVPP_USE_STL_COROUTINES)
                            JoinInTask(std::bind((*event_queue_nolock_function))).get();
#else
                            (*event_queue_nolock_function)();
#endif
                        }
                    }
                    else
                    {
                        assert(0);
                    }
                }
                catch (const EventException& ex)
                {
                    EVENT_INFO("%s", ex.what());
                }
            }
        }
#else
        std::vector<Handler> functors;
        {
            std::lock_guard<std::mutex> lock(event_queue_nolock_mutex);
            functors.swap(event_queue_nolock);
        }

        for (const auto& function : functors)
        {
            try
            {
#if defined(EVPP_USE_STL_COROUTINES)
                JoinInTask(std::bind(function)).get();
#else
                function();
#endif
            }
            catch (const EventException& ex)
            {
                EVENT_INFO("%s", ex.what());
            }
        }
#endif
    }

    void EventQueue::EventNotifyEx()
    {
#if defined(EVPP_USE_CAMERON314_CONCURRENTQUEUE)
        while (event_queue_lock->try_dequeue(event_queue_lock_function))
        {
            try
            {
#if defined(EVPP_USE_STL_COROUTINES)
                JoinInTask(std::bind(event_queue_lock_function)).get();
#else
                event_queue_lock_function();
#endif
            }
            catch (const EventException& ex)
            {
                EVENT_INFO("%s", ex.what());
            }

            if (event_locking.notify())
            {
                continue;
            }
        }
#elif defined(EVPP_USE_BOOST_LOCKFREE_QUEUE)
        while (event_queue_lock->pop(event_queue_lock_function))
        {
            if (nullptr != event_queue_lock_function)
            {
                try
                {
                    if (evebt_queue_lock_function_count.fetch_sub(1, std::memory_order_relaxed))
                    {
                        SafeReleaseHandler destroy_object(std::addressof(event_queue_lock_function));
                        {
#if defined(EVPP_USE_STL_COROUTINES)
                            JoinInTask(std::bind((*event_queue_lock_function))).get();
#else
                            (*event_queue_lock_function)();
#endif
                        }
                    }
                    else
                    {
                        assert(0);
                    }
                }
                catch (const EventException& ex)
                {
                    EVENT_INFO("%s", ex.what());
                }

                if (event_semaphore->StopWaiting())
                {
                    continue;
                }
            }
        }
#else
        std::vector<Handler> functors;
        {
            std::lock_guard<std::mutex> lock(event_queue_lock_mutex);
            functors.swap(event_queue_lock);
        }

        for (const auto& function : functors)
        {
            if (evebt_queue_lock_function_count.fetch_sub(1, std::memory_order_relaxed))
            {
                try
                {
#if defined(EVPP_USE_STL_COROUTINES)
                    JoinInTask(std::bind(function)).get();
#else
                    function();
#endif
                }
                catch (const EventException& ex)
                {
                    EVENT_INFO("%s", ex.what());
                }

                if (event_semaphore->StopWaiting())
                {
                    continue;
                }
            }
            else
            {
                assert(0);
            }
        }
#endif
    }
}