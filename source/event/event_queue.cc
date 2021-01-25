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

    void EventQueue::OnWorkNotify(event_work* handler)
    {
        
    }

    void EventQueue::OnAfterWorkNotify(event_work* handler, int status)
    {
        
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