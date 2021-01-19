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
    class EventQueue
    {
    public:
        explicit EventQueue(EventLoop* loop);
        virtual ~EventQueue();
    public:
        bool CreateQueue();
    public:
        //bool RunInLoop(Functor function);
        bool RunInLoop(const Functor& function);
    private:
        bool SendAsyncNotify(const Functor& function);
        void RecvAsyncNotify();
    private:
        EventLoop*                                                                                          event_loop;
        std::unique_ptr<EventPipe>                                                                          event_pipe;
        std::unique_ptr<moodycamel::ConcurrentQueue<Functor, Traits>>                                       event_async;
    };
}
#endif // __event_queue_H__