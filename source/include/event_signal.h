#ifndef __EVENT_SIGNAL_H__
#define __EVENT_SIGNAL_H__
#include <event_config.h>
namespace Evpp
{
    class EventLoop;
    class EventSignal
    {
    public:
        explicit EventSignal(EventLoop* loop, const Handler& function = Handler());
        virtual ~EventSignal();
    public:
        bool InitialSignal();
        bool CreaterSignal(const i32 signum = SIGHUP);
        bool DestroySignal();
    public:
        void SetSignalCallback(const Handler& function);
    private:
        static void OnNotify(event_signal* handler, int signum);
        void OnNotify();
    private:
        EventLoop* event_base;
        event_signal* event_signaler;
        Handler                                                                 event_callback;
    };
}
#endif // __EVENT_SIGNAL_H__
