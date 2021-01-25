#include <event_work_queue.h>
#include <event_loop.h>
namespace Evpp
{
    EventWorkQueue::EventWorkQueue(EventLoop* loop) :
        event_base(loop)
    {
        
    }

    EventWorkQueue::~EventWorkQueue()
    {

    }

    bool EventWorkQueue::AssignWorkQueue(EventLoop* loop)
    {
        printf("AssignWorkQueue ID£º%d\n", event_base->EventThreadId());
        if (nullptr != event_base)
        {
           return 0 == uv_queue_work(loop->EventBasic(), new event_work({ this }), &EventWorkQueue::DefaultWorkNotify, &EventWorkQueue::DefaultAfterWorkNotify);
        }
        return false;
    }

    bool EventWorkQueue::AssignWorkQueue()
    {
        printf("AssignWorkQueue ID£º%d\n", event_base->EventThreadId());
        if (nullptr != event_base)
        {
            return 0 == uv_queue_work(event_base->EventBasic(), new event_work({ this }), &EventWorkQueue::DefaultWorkNotify, &EventWorkQueue::DefaultAfterWorkNotify);
        }
        return false;
    }

    void EventWorkQueue::SetWorkCallback(const Handler& function)
    {
        if (nullptr == work_callback)
        {
            work_callback = function;
        }
    }

    void EventWorkQueue::SetAfterWorkCallback(const Handler& function)
    {
        if (nullptr == afterwork_callback)
        {
            afterwork_callback = function;
        }
    }

    void EventWorkQueue::OnWorkNotify(event_work* handler)
    {
        printf("OnWorkNotify ID£º%d\n", event_base->EventThreadId());
        if (nullptr != work_callback)
        {
            work_callback();
        }
    }

    void EventWorkQueue::OnAfterWorkNotify(event_work* handler, int status)
    {
        printf("OnAfterWorkNotify ID£º%d\n", event_base->EventThreadId());
        if (nullptr != afterwork_callback)
        {
            afterwork_callback();
        }
    }

    void EventWorkQueue::DefaultWorkNotify(event_work* handler)
    {
        if (nullptr != handler)
        {
            EventWorkQueue* watcher = static_cast<EventWorkQueue*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnWorkNotify(handler);
                }
            }
        }
    }

    void EventWorkQueue::DefaultAfterWorkNotify(event_work* handler, int status)
    {
        if (nullptr != handler)
        {
            EventWorkQueue* watcher = static_cast<EventWorkQueue*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnAfterWorkNotify(handler, status);

                    delete handler;
                    handler = nullptr;
                }
            }
        }
    }
}