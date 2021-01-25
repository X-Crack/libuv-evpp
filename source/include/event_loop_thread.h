#ifndef __EVENT_LOOP_THREAD_H__
#define __EVENT_LOOP_THREAD_H__
#include <config.h>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
namespace Evpp
{
    class EventLoop;
    class EventShare;
    class EventStatus;
    class EventLoopThread : public EventStatus
    {
    public:
        explicit EventLoopThread(const u96 index);
        explicit EventLoopThread(EventLoop* loop, const std::shared_ptr<EventShare>& share, const u96 index);
        virtual ~EventLoopThread();
    public:
        bool CreaterEventLoopThread(bool wait = true);
        bool Join();
        EventLoop* GetEventLoop();
    private:
        bool AvailableEvent();
        void Run();
    private:
        EventLoop*                                      event_base;
        std::shared_ptr<EventShare>                     event_share;
        u96                                             event_index;
        std::shared_ptr<EventLoop>                      loop;
        std::unique_ptr<std::thread>                    loop_thread;
        std::condition_variable							cv_signal;
        std::mutex										cv_mutex;
    };
}
#endif // __EVENT_LOOP_THREAD_H__