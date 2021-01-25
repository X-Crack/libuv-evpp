#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__
#include <config.h>
#include <memory>
namespace moodycamel
{
    struct ConcurrentQueueDefaultTraits;
    template<typename T, typename Traits> class ConcurrentQueue;
    template<typename T, typename Traits> class BlockingConcurrentQueue;
}
namespace Evpp
{
    struct Traits;
    class EventLoop;
    class EventAsync;
    class EventWatcher
    {
    public:
        explicit EventWatcher(EventLoop* loop);
        virtual ~EventWatcher();
    public:
        bool CreateQueue();
    public:
        bool RunInLoop(const Functor& function);
        bool RunInLoop(Functor&& function);
        bool RunInLoopEx(const Handler& function);
        bool RunInLoopEx(Handler&& function);
    private:
        bool SendAsyncNotify(const Functor& function);
        void RecvAsyncNotify();
        bool SendAsyncNotifyEx(const Handler& function);
        void RecvAsyncNotifyEx();
    private:
        EventLoop*                                                                                          event_base;
        std::unique_ptr<EventAsync>                                                                         event_async_;
        std::unique_ptr<EventAsync>                                                                         event_async_ex_;
        std::unique_ptr<moodycamel::ConcurrentQueue<Functor, Traits>>                                       nolock_queue;
        std::unique_ptr<moodycamel::ConcurrentQueue<Handler, Traits>>                                       nolock_queue_ex;
    };
}
#endif // __event_queue_H__