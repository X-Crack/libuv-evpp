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
        event_async(std::make_unique<moodycamel::ConcurrentQueue<Functor, Traits>>())
    {
        
    }

    EventQueue::~EventQueue()
    {

    }

    bool EventQueue::CreateQueue()
    {
        return event_pipe->CreatePipe();
    }

//     bool EventQueue::RunInLoop(Functor function)
//     {
//         if (nullptr != event_loop && nullptr != event_pipe)
//         {
//             if (event_loop->SelftyThread())
//             {
//                 return function();
//             }
//             return SendAsyncNotify(std::move(function));
//         }
//         return false;
//     }

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

}