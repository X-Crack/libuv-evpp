#include <event_watcher.h>
#include <event_loop.h>
#include <event_async.h>
#include <blockingconcurrentqueue.h>
#include <concurrentqueue.h>
#include <event_coroutine.h>
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
        event_base(loop),
        event_async_(std::make_unique<EventAsync>(loop, std::bind(&EventWatcher::RecvAsyncNotify, this))),
        event_async_ex_(std::make_unique<EventAsync>(loop, std::bind(&EventWatcher::RecvAsyncNotifyEx, this))),
        nolock_queue(std::make_unique<moodycamel::ConcurrentQueue<Functor, Traits>>()),
        nolock_queue_ex(std::make_unique<moodycamel::ConcurrentQueue<Handler, Traits>>())
    {

    }

    EventWatcher::~EventWatcher()
    {

    }

    bool EventWatcher::CreaterQueue()
    {
        if (event_async_->CreatePipe())
        {
            if (event_async_ex_->CreatePipe())
            {
                return true;
            }
            return false;
        }
        return false;
    }

    bool EventWatcher::DestroyQueue()
    {
        return nolock_queue->size_approx() || nolock_queue_ex->size_approx();
    }

    bool EventWatcher::RunInLoop(const Functor& function)
    {
        if (nullptr != event_base && nullptr != event_async_)
        {
            if (event_base->EventThread())
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
        if (nullptr != event_base && nullptr != event_async_)
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
        if (nullptr != event_async_)
        {
            while (false == nolock_queue->try_enqueue(function));

            return event_async_->ExecNotify();
        }
        return false;
    }

    void EventWatcher::RecvAsyncNotify()
    {
        Functor function;

        while (nolock_queue->try_dequeue(function))
        {
            if (!function())
            {
                printf("AsyncDeQueue Error\n");
            }
        }
    }

    bool EventWatcher::SendAsyncNotifyEx(const Handler& function)
    {
        if (nullptr != event_async_ex_)
        {
            while (false == nolock_queue_ex->try_enqueue(function));

            return event_async_ex_->ExecNotify();
        }
        return false;
    }

    void EventWatcher::RecvAsyncNotifyEx()
    {
        Handler function;

        while (nolock_queue_ex->try_dequeue(function))
        {
            function();
        }
    }
}