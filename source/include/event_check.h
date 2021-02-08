#ifndef __EVENT_CHECK_H__
#define __EVENT_CHECK_H__
#include <event_config.h>

namespace Evpp
{
    class EventLoop;
    class EventCheck
    {
    public:
        explicit EventCheck(EventLoop* loop);
        virtual ~EventCheck();
    public:
        bool CreaterCheck();
        bool DestroyCheck();
        void SetCallback(const EventLoopHandler& callback);
    private:
        bool InitialCheck();
    private:
        static void OnNotify(event_check* handler);
        void OnNotify();
    private:
        EventLoop*                                                  event_base;
        event_check*                                                event_check_handler;
        EventLoopHandler                                            event_check_callback;
    };
}
#endif // __EVENT_CHECK_H__
