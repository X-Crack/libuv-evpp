#ifndef __EVENT_ASYNC_H__
#define __EVENT_ASYNC_H__
#include <event_config.h>
#include <atomic>
namespace Evpp
{
    class EventLoop;
    class EVPP_EXPORT EventAsync
    {
    public:
        explicit EventAsync(EventLoop* loop, const Handler& handler);
        virtual ~EventAsync();
    public:
        bool CreaterAsync();
        bool DestroyAsync();
        bool ExecNotify();
    private:
        static void OnNotify(event_async* handler);
        void OnNotify();
    private:
        static void DefaultClose(event_handle* handler);
        void OnClose(event_handle* handler);
    private:
        EventLoop*                                              event_base;
        event_async*                                            event_queue;
        Handler                                                 event_handler;
    };
}
#endif // __EVENT_ASYNC_H__
