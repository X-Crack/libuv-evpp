#include <event_queue.h>
#include <event_loop.h>
namespace Evpp
{
    EventQueue::EventQueue(EventLoop* loop) :
        event_base(loop)
    {
        
    }

    EventQueue::~EventQueue()
    {

    }

    bool EventQueue::AssignWorkQueue(EventLoop* loop)
    {
        if (nullptr != event_base)
        {
           return 0 == uv_queue_work(loop->EventBasic(), new event_work({ this }), &EventQueue::DefaultWorkNotify, &EventQueue::DefaultAfterWorkNotify);
        }
        return false;
    }

    bool EventQueue::AssignWorkQueue()
    {
        if (nullptr != event_base)
        {
            return 0 == uv_queue_work(event_base->EventBasic(), new event_work({ this }), &EventQueue::DefaultWorkNotify, &EventQueue::DefaultAfterWorkNotify);
        }
        return false;
    }

    void EventQueue::SetWorkCallback(const Handler& function)
    {
        if (nullptr == work_callback)
        {
            work_callback = function;
        }
    }

    void EventQueue::SetAfterWorkCallback(const Handler& function)
    {
        if (nullptr == afterwork_callback)
        {
            afterwork_callback = function;
        }
    }

    void EventQueue::OnWorkNotify(event_work* handler)
    {
        if (nullptr != work_callback)
        {
            work_callback();
        }
    }

    void EventQueue::OnAfterWorkNotify(event_work* handler, int status)
    {
        if (nullptr != afterwork_callback)
        {
            afterwork_callback();
        }
    }

    void EventQueue::DefaultWorkNotify(event_work* handler)
    {
        if (nullptr != handler)
        {
            EventQueue* watcher = static_cast<EventQueue*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnWorkNotify(handler);
                }
            }
        }
    }

    void EventQueue::DefaultAfterWorkNotify(event_work* handler, int status)
    {
        if (nullptr != handler)
        {
            EventQueue* watcher = static_cast<EventQueue*>(handler->data);
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