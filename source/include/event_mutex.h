#ifndef __EVENT_MUTEX_H__
#define __EVENT_MUTEX_H__
#include <event_config.h>
#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>
namespace Evpp
{
    class EVPP_EXPORT EventMutex
    {
    public:
        explicit EventMutex(event_mutex* lock);
        virtual ~EventMutex();
    private:
        event_mutex*                                    event_lock_mutex;
    };

    class EventLocking
    {
    public:
        explicit EventLocking();
        virtual ~EventLocking();
    public:
        bool dowait(const std::chrono::milliseconds& delay = std::chrono::milliseconds(UINT32_MAX));
        bool notify();
    private:
        bool advise();
    public:
        std::atomic<u96>                                        event_cv_flag;
        std::condition_variable                                 event_cv_signal;
        std::mutex                                              event_cv_mutex;
    };
}
#endif // __event_mutex_H__
