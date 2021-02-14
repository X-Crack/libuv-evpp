#ifndef __EVENT_LOOP_THREAD_H__
#define __EVENT_LOOP_THREAD_H__
#include <event_config.h>
namespace boost
{
    class thread;
}
namespace Evpp
{
    class EventLoop;
    class EventShare;
    class EventStatus;
    class EventSemaphore;
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
        std::unique_ptr<EventSemaphore>                 event_semaphore;
        std::shared_ptr<EventLoop>                      loop;
#if defined(EVPP_USE_STL_THREAD)
        std::unique_ptr<std::thread>                    loop_thread;

#elif defined(EVPP_USE_BOOST_THREAD)
        std::unique_ptr<boost::thread>                  loop_thread;
#else
        std::unique_ptr<event_thread>                   loop_thread;
#endif
        std::condition_variable                         cv_signal;
        std::mutex                                      cv_mutex;
    };
}
#endif // __EVENT_LOOP_THREAD_H__