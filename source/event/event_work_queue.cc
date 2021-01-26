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
        if (nullptr != event_base)
        {
            return 0 == uv_queue_work(loop->EventBasic(), new event_work({ this }), &EventWorkQueue::OnCreaterNotify, &EventWorkQueue::OnDestroyNotify);
        }
        return false;
    }

    bool EventWorkQueue::AssignWorkQueue()
    {
        if (nullptr != event_base)
        {
            return 0 == uv_queue_work(event_base->EventBasic(), new event_work({ this }), &EventWorkQueue::OnCreaterNotify, &EventWorkQueue::OnDestroyNotify);
        }
        return false;
    }

    void EventWorkQueue::SetCreaterCallback(const CreaterWorkHandler& function)
    {
        if (nullptr == creater_callback)
        {
            creater_callback = function;
        }
    }

    void EventWorkQueue::SetDestroyCallback(const DestroyWorkHandler& function)
    {
        if (nullptr == destroy_callback)
        {
            destroy_callback = function;
        }
    }

    void EventWorkQueue::OnCreaterNotify()
    {
        if (nullptr != creater_callback)
        {
            creater_callback(event_base);
        }
    }

    void EventWorkQueue::OnDestroyNotify(int status)
    {
        if (nullptr != destroy_callback)
        {
            destroy_callback(event_base, status);
        }
    }

    void EventWorkQueue::OnCreaterNotify(event_work* handler)
    {
        if (nullptr != handler)
        {
            EventWorkQueue* watcher = static_cast<EventWorkQueue*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    return watcher->OnCreaterNotify();
                }
            }
        }
    }

    void EventWorkQueue::OnDestroyNotify(event_work* handler, int status)
    {
        if (nullptr != handler)
        {
            EventWorkQueue* watcher = static_cast<EventWorkQueue*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    delete handler;
                    handler = nullptr;
                    return watcher->OnDestroyNotify(status);
                }
            }
        }
    }
}