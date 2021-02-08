#ifndef __EVENT_MUTEX_H__
#define __EVENT_MUTEX_H__
#include <event_config.h>
#include <atomic>
#include <semaphore>
namespace Evpp
{
    class EventMutex
    {
    public:
        explicit EventMutex();
    public:
        bool try_lock() noexcept;
        bool try_unlock() noexcept;
        bool lock() noexcept;
        bool unlock() noexcept;
    private:
        std::atomic<u96>                                                counter;
    };

    class EventSemaphore
    {
    public:
        explicit EventSemaphore();
    public:
        void lock() noexcept;
        void unlock() noexcept;
    private:
        std::binary_semaphore                                           semaphore;
    };
}
#endif // __event_mutex_H__
