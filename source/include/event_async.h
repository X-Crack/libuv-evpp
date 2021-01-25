#ifndef __EVENT_ASYNC_H__
#define __EVENT_ASYNC_H__
#include <config.h>
#include <event_untlity.h>
namespace Evpp
{
    class EventLoop;
    class EventAsync
    {
    public:
        explicit EventAsync(EventLoop* loop, const Handler& handler);
        virtual ~EventAsync();
    public:
        bool CreatePipe();
        bool ExecNotify();
    private:
        static void OnNotify(event_async* handler);
        void OnNotify();
    private:
        EventLoop*                                      event_loop;
        event_async*                                    event_async_;
        Handler                                         cv_handler;
    };
}
#endif // __EVENT_ASYNC_H__