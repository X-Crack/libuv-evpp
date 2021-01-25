#include <event_watcher.h>
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

    EventWatcher::EventWatcher(EventLoop* loop) :
        event_loop(loop),
        event_pipe(std::make_unique<EventPipe>(loop, std::bind(&EventWatcher::RecvAsyncNotify, this))),
        event_pipe_ex(std::make_unique<EventPipe>(loop, std::bind(&EventWatcher::RecvAsyncNotifyEx, this))),
        event_async(std::make_unique<moodycamel::ConcurrentQueue<Functor, Traits>>()),
        event_async_ex(std::make_unique<moodycamel::ConcurrentQueue<Handler, Traits>>())
    {

    }

    EventWatcher::~EventWatcher()
    {

    }

    bool EventWatcher::CreateQueue()
    {
        return event_pipe->CreatePipe() && event_pipe_ex->CreatePipe();
    }

    bool EventWatcher::RunInLoop(const Functor& function)
    {
        if (nullptr != event_loop && nullptr != event_pipe)
        {
            if (event_loop->EventThread())
            {
                return function();
            }
            return SendAsyncNotify(function);
        }
        return false;
    }

    bool EventWatcher::RunInLoop(Functor&& function)
    {
        return RunInLoop(function);
    }

    bool EventWatcher::RunInLoopEx(const Handler& function)
    {
        if (nullptr != event_loop && nullptr != event_pipe)
        {
            return SendAsyncNotifyEx(function);
        }
        return false;
    }

    bool EventWatcher::RunInLoopEx(Handler&& function)
    {
        return RunInLoopEx(function);
    }

    bool EventWatcher::SendAsyncNotify(const Functor& function)
    {
        if (nullptr != event_pipe)
        {
            while (false == event_async->try_enqueue(function));

            return event_pipe->ExecNotify();
        }
        return false;
    }

    void EventWatcher::RecvAsyncNotify()
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

    bool EventWatcher::SendAsyncNotifyEx(const Handler& function)
    {
        if (nullptr != event_pipe_ex)
        {
            while (false == event_async_ex->try_enqueue(function));

            return event_pipe_ex->ExecNotify();
        }
        return false;
    }

    void EventWatcher::RecvAsyncNotifyEx()
    {
        Handler function;

        while (event_async_ex->try_dequeue(function))
        {
            function();
        }
    }
}