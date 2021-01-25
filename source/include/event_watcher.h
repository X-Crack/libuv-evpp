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
    class EventPipe;
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
        EventLoop*                                                                                          event_loop;
        std::unique_ptr<EventPipe>                                                                          event_pipe;
        std::unique_ptr<EventPipe>                                                                          event_pipe_ex;
        std::unique_ptr<moodycamel::ConcurrentQueue<Functor, Traits>>                                       event_async;
        std::unique_ptr<moodycamel::ConcurrentQueue<Handler, Traits>>                                       event_async_ex;
    };
}
#endif // __event_queue_H__