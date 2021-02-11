#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__
#include <event_config.h>
#include <memory>
#include <atomic>
#include <mutex>
#if defined(EVPP_USE_CAMERON314_CONCURRENTQUEUE)
namespace moodycamel
{
    struct ConcurrentQueueDefaultTraits;
    template<typename T, typename Traits> class ConcurrentQueue;
    template<typename T, typename Traits> class BlockingConcurrentQueue;
}
#elif defined(EVPP_USE_BOOST_LOCKFREE_QUEUE)
namespace boost
{
    namespace lockfree
    {
#ifdef BOOST_NO_CXX11_VARIADIC_TEMPLATES
        template <typename T, class A0, class A1, class A2>
#else
        template <typename T, typename ...Options>
#endif
        class queue;
    }
}
#endif

namespace Evpp
{
    class EventLoop;
    class EventAsync;
    class EventMutex;
    class EventQueueTraits;
    class EventQueue
    {
    public:
        explicit EventQueue(EventLoop* base);
        virtual ~EventQueue();
    public:
        bool CreaterQueue();
        bool DestroyQueue();
        bool RunInLoop(const Handler& function);
        bool RunInLoopEx(const Handler& function);
        bool RunInQueue(const Handler& function);
    private:
        bool EmplaceQueue(const Handler& function);
        bool EmplaceQueueEx(const Handler& function);
    private:
        void EventNotify();
        void EventNotifyEx();
    private:
        EventLoop*                                                                          event_base;
        std::unique_ptr<EventAsync>                                                         event_queue;         // 异步
        std::unique_ptr<EventAsync>                                                         event_queue_ex;      // 同步
        std::unique_ptr<EventMutex>                                                         event_queue_mutex;
#if defined(EVPP_USE_CAMERON314_CONCURRENTQUEUE)
        std::unique_ptr<moodycamel::ConcurrentQueue<Handler, EventQueueTraits>>             event_queue_nolock;
        std::unique_ptr<moodycamel::ConcurrentQueue<Handler, EventQueueTraits>>             event_queue_lock;
        Handler                                                                             event_queue_nolock_function;
        Handler                                                                             event_queue_lock_function;
#elif defined(EVPP_USE_BOOST_LOCKFREE_QUEUE)
        std::unique_ptr<boost::lockfree::queue<Handler*>>                                   event_queue_nolock;
        std::unique_ptr<boost::lockfree::queue<Handler*>>                                   event_queue_lock;
        Handler*                                                                            event_queue_nolock_function;
        Handler*                                                                            event_queue_lock_function;
        std::atomic<u32>                                                                    evebt_queue_nolock_function_count;
        std::atomic<u32>                                                                    evebt_queue_lock_function_count;
#else
        std::vector<Handler>                                                                event_queue_nolock;
        std::vector<Handler>                                                                event_queue_lock;
        std::mutex                                                                          event_queue_nolock_mutex;
        std::mutex                                                                          event_queue_lock_mutex;
#endif
#ifdef EVPP_USE_BOOST_LOCKFREE_QUEUE
    private:
        class SafeReleaseHandler
        {
        public:
            explicit SafeReleaseHandler(Handler** h) : handler(h) {};
            virtual ~SafeReleaseHandler()
            {
                if (nullptr != *handler)
                {
                    delete *handler;
                    *handler = nullptr;
                }
            }
        private:
            Handler**                                                                        handler;
        };
#endif
    };
}

#endif // __EVENT_QUEUE_H__