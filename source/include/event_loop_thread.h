#ifndef __EVENT_LOOP_THREAD_H__
#define __EVENT_LOOP_THREAD_H__
#include <event_config.h>
//#define EVPP_USE_UV_THREAD 1
namespace Evpp
{
    class EventLoop;
    class EventShare;
    class EventStatus;
    class EventLoopThread final : public EventStatus
    {
    public:
        explicit EventLoopThread(EventLoop* base, event_loop* loop, const u96 index);
        explicit EventLoopThread(EventLoop* base, const std::shared_ptr<EventShare>& share, const u96 index);
        virtual ~EventLoopThread();
    public:
        bool CreaterSubThread();
        bool DestroyThread();
        EventLoop* GetEventLoop();
    private:
        bool StopDispatch();
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
        std::shared_ptr<EventShare>                     event_share;
        u96                                             event_index;
        std::shared_ptr<EventLoop>                      loop;
#ifdef EVPP_USE_STL_THREAD
        std::unique_ptr<std::thread>                    loop_thread;

#else
        std::unique_ptr<event_thread>                   loop_thread;
#endif
        std::atomic<u32>                                loop_exit;
        std::condition_variable                         cv_signal;
        std::mutex                                      cv_mutex;
    };
}
#endif // __EVENT_LOOP_THREAD_H__
