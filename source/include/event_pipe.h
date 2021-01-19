#ifndef __EVENT_PIPE_H__
#define __EVENT_PIPE_H__
#include <config.h>
#include <event_untlity.h>
namespace Evpp
{
    class EventLoop;
    class EventPipe
    {
    public:
        explicit EventPipe(EventLoop* loop, const Handler& handler);
        virtual ~EventPipe();
    public:
        bool CreatePipe();
        bool ExecNotify();
    private:
        static void OnNotify(event_async* handler);
        void OnNotify();
    private:
        EventLoop*                                      event_loop;
        event_async*                                    event_pipe;
        Handler                                         cv_handler;
    };
}
#endif // __EVENT_PIPE_H__