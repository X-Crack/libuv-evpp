#include <event_pipe.h>
#include <event_loop.h>
namespace Evpp
{
    EventPipe::EventPipe(EventLoop* loop, const Handler& handler) : 
        event_loop(loop), 
        event_pipe(new uv_async_t()), 
        cv_handler(handler)
    {
        event_pipe->data = this;
    }

    EventPipe::~EventPipe()
    {
        if (event_pipe)
        {
            delete event_pipe;
            event_pipe = nullptr;
        }
    }

    bool EventPipe::CreatePipe()
    {
        if (nullptr != event_loop && nullptr != event_pipe)
        {
            return 0 == uv_async_init(event_loop->EventBasic(), event_pipe, &EventPipe::OnNotify);
        }
        return false;
    }

    bool EventPipe::ExecNotify()
    {
        if (nullptr != event_pipe && nullptr != event_pipe->data)
        {
            return 0 == uv_async_send(event_pipe);
        }
        return false;
    }

    void EventPipe::OnNotify(event_async* handler)
    {
        if (nullptr != handler)
        {
            EventPipe* watcher = static_cast<EventPipe*>(handler->data);
            {
                if (nullptr != watcher)
                {
                    watcher->OnNotify();
                }
            }
        }
    }
    void EventPipe::OnNotify()
    {
        if (nullptr != cv_handler)
        {
            cv_handler();
        }
    }
}