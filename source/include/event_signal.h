#ifndef __EVENT_SIGNAL_H__
#define __EVENT_SIGNAL_H__
#include <config.h>
namespace Evpp
{
    class EventLoop;
    class EventSignal
    {
    public:
        explicit EventSignal(EventLoop* loop);
        virtual ~EventSignal();
    public:
        bool InitialSignal();
        bool CreaterSignal(const i32 signum);
        bool DestroySignal();
    private:
        static void OnNotify(event_signal* handler, int signum);
        void OnNotify();
    private:
        EventLoop*                                                              event_loop;
        event_signal*                                                           event_signaler;
    };
}
#endif // __EVENT_SIGNAL_H__