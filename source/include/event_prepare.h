#ifndef __EVENT_PREPARE_H__
#define __EVENT_PREPARE_H__
#include <event_config.h>
namespace Evpp
{
    class EventLoop;
    class EVPP_EXPORT EventPrepare
    {
    public:
        explicit EventPrepare(EventLoop* loop);
        virtual ~EventPrepare();
    public:
        bool CreaterPrepare();
        bool DestroyPrepare();
        void SetCallback(const EventLoopHandler& callback);
    private:
        bool InitialPrepare();
    private:
        static void OnNotify(event_prepare* handler);
        void OnNotify();
    private:
        EventLoop*                                                  event_base;
        event_prepare*                                              event_prepare_handler;
        EventLoopHandler                                            event_prepare_callback;
    };
}
#endif // __EVENT_PREPARE_H__
