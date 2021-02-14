#ifndef __EVENT_MUTEX_H__
#define __EVENT_MUTEX_H__
#include <event_config.h>
#include <atomic>
#include <chrono>
namespace Evpp
{
    class EventSemaphore
    {
    public:
        explicit EventSemaphore();
        virtual ~EventSemaphore();
    public:
        bool StarWaiting(const std::chrono::milliseconds& delay = std::chrono::milliseconds(3000));
        bool StopWaiting();
    private:
        std::atomic<u96>                                                                    event_cv_pending;
    };
}
#endif // __event_mutex_H__
