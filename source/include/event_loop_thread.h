#ifndef __EVENT_LOOP_THREAD_H__
#define __EVENT_LOOP_THREAD_H__
#include <event_config.h>
namespace Evpp
{
    class EventLoop;
    class EventShare;
    class EventStatus;
    class EventMutex;
    class EventLoopThread final : public EventStatus
    {
    public:
        explicit EventLoopThread(EventLoop* base, event_loop* loop, const u96 index);
        explicit EventLoopThread(EventLoop* base, const std::shared_ptr<EventShare>& share, const u96 index);
        virtual ~EventLoopThread();
    public:
        bool CreaterThread();
        bool DestroyThread();
        EventLoop* GetEventLoop();
    private:
        void CoroutineInThread();
        bool CoroutineDispatch();
    private:
        bool AvailableEvent();
        bool Join();
    private:
        static void WatcherCoroutineInThread(void* handler);
    private:
        EventLoop*                                      event_base;
        u96                                             event_index;
        std::shared_ptr<EventLoop>                      loop;
#ifdef EVPP_USE_STL_THREAD
        std::unique_ptr<std::thread>                    loop_thread;

#else
        std::unique_ptr<event_thread>                   loop_thread;
#endif
        std::unique_ptr<EventMutex>                     loop_mutex;
        std::condition_variable                         cv_signal;
        std::mutex                                      cv_mutex;
    };
}
#endif // __EVENT_LOOP_THREAD_H__
