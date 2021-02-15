#ifndef __EVENT_IDLE_H__
#define __EVENT_IDLE_H__
#include <event_config.h>
namespace Evpp
{
    class EventLoop;
    class EVPP_EXPORT EventIdle
    {
    public:
        explicit EventIdle(EventLoop* loop);
        virtual ~EventIdle();
    public:
        bool CreaterIdle();
        bool DestroyIdle();
        void SetCallback(const EventLoopHandler& callback);
    private:
        bool InitialIdle();
    private:
        static void OnNotify(event_idle* handler);
        void OnNotify();
    private:
        EventLoop*                                                  event_base;
        event_idle*                                                 event_idle_handler;
        EventLoopHandler                                            event_idle_callback;
    };
}
#endif // __EVENT_IDLE_H__
