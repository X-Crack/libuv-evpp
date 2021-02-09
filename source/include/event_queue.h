#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__
#include <event_config.h>
#include <memory>
#include <atomic>
#include <mutex>
#ifdef EVPP_USE_CAMERON314_CONCURRENTQUEUE
namespace moodycamel
{
    struct ConcurrentQueueDefaultTraits;
    template<typename T, typename Traits> class ConcurrentQueue;
    template<typename T, typename Traits> class BlockingConcurrentQueue;
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
        std::unique_ptr<EventAsync>                                                         event_queue;         // “Ï≤Ω
        std::unique_ptr<EventAsync>                                                         event_queue_ex;      // Õ¨≤Ω
        std::unique_ptr<EventMutex>                                                         event_queue_mutex;
#ifdef EVPP_USE_CAMERON314_CONCURRENTQUEUE
        std::unique_ptr<moodycamel::ConcurrentQueue<Handler, EventQueueTraits>>             event_queue_nolock;
        std::unique_ptr<moodycamel::ConcurrentQueue<Handler, EventQueueTraits>>             event_queue_lock;
#else
        std::vector<Handler>                                                                event_queue_nolock;
        std::vector<Handler>                                                                event_queue_lock;
#endif
        Handler                                                                             event_queue_nolock_function;
        Handler                                                                             event_queue_lock_function;
        std::mutex                                                                          event_queue_nolock_mutex;
        std::mutex                                                                          event_queue_lock_mutex;
    };
}

#endif // __EVENT_QUEUE_H__