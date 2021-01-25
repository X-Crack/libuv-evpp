#ifndef __EVENT_LOOP_THREAD_H__
#define __EVENT_LOOP_THREAD_H__
#include <config.h>
namespace Evpp
{
    class EventLoop;
    class EventShare;
    class EventStatus;
    class EventWorkQueue;
    class EventLoopThread : public EventStatus
    {
    public:
        explicit EventLoopThread(const u96 index);
        explicit EventLoopThread(EventLoop* loop, const std::shared_ptr<EventShare>& share, const u96 index);
        virtual ~EventLoopThread();
    public:
        bool CreaterThread(bool wait);
        EventLoop* GetEventLoop();
    private:
        bool CreaterThread();
        bool DestroyThread();
    private:
        void ThreadRun();
    private:
        static void ThreadRun(void* handler);
    private:
        EventLoop*                                      event_base;
        std::shared_ptr<EventShare>                     event_share;
        std::unique_ptr<event_thread>                   event_thread;
        u96                                             event_index;
        std::shared_ptr<EventLoop>                      loops_base;
    };
}
#endif // __EVENT_LOOP_THREAD_H__