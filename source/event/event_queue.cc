#include <event_queue.h>
#include <event_loop.h>
#include <event_pipe.h>
#include <blockingconcurrentqueue.h>
#include <concurrentqueue.h>
namespace Evpp
{
    struct Traits : public moodycamel::ConcurrentQueueDefaultTraits
    {
    public:
        static const size_t BLOCK_SIZE = 4096;
        static const size_t EXPLICIT_INITIAL_INDEX_SIZE = 256;
        static const size_t IMPLICIT_INITIAL_INDEX_SIZE = 256;
        static const std::uint32_t EXPLICIT_CONSUMER_CONSUMPTION_QUOTA_BEFORE_ROTATE = 1024;
    };

    EventQueue::EventQueue(EventLoop* loop) :
        event_loop(loop),
        event_pipe(std::make_unique<EventPipe>(loop, std::bind(&EventQueue::RecvAsyncNotify, this))),
        event_pipe_ex(std::make_unique<EventPipe>(loop, std::bind(&EventQueue::RecvAsyncNotifyEx, this))),
        event_async(std::make_unique<moodycamel::ConcurrentQueue<Functor, Traits>>()),
        event_async_ex(std::make_unique<moodycamel::ConcurrentQueue<Handler, Traits>>())
    {

    }

    EventQueue::~EventQueue()
    {

    }

    bool EventQueue::CreateQueue()
    {
        return event_pipe->CreatePipe() && event_pipe_ex->CreatePipe();
    }

    bool EventQueue::RunInLoop(const Functor& function)
    {
        if (nullptr != event_loop && nullptr != event_pipe)
        {
            if (event_loop->SelftyThread())
            {
                return function();
            }
            return SendAsyncNotify(function);
        }
        return false;
    }

    bool EventQueue::RunInLoop(Functor&& function)
    {
        return RunInLoop(function);
    }

    bool EventQueue::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_loop && nullptr != event_pipe)
        {
            return SendAsyncNotifyEx(function);
        }
        return false;
    }

    bool EventQueue::RunInLoopEx(Handler&& function)
    {
        return RunInLoopEx(function);
    }

    bool EventQueue::SendAsyncNotify(const Functor& function)
    {
        if (nullptr != event_pipe)
        {
            while (false == event_async->try_enqueue(function));

            return event_pipe->ExecNotify();
        }
        return false;
    }

    void EventQueue::RecvAsyncNotify()
    {
        Functor function;

        while (event_async->try_dequeue(function))
        {
            if (!function())
            {
                printf("AsyncDeQueue Error\n");
            }
        }
    }

    bool EventQueue::SendAsyncNotifyEx(const Handler& function)
    {
        if (nullptr != event_pipe_ex)
        {
            while (false == event_async_ex->try_enqueue(function));

            return event_pipe_ex->ExecNotify();
        }
        return false;
    }

    void EventQueue::RecvAsyncNotifyEx()
    {
        Handler function;

        while (event_async_ex->try_dequeue(function))
        {
            function();
        }
    }
}