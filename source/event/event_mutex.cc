#include <event_mutex.h>

namespace Evpp
{
    EventMutex::EventMutex() : counter(1)
    {

    }

    bool EventMutex::try_lock() noexcept
    {
        return 1 == counter.load(std::memory_order_relaxed);
    }

    bool EventMutex::try_unlock() noexcept
    {
        return 1 == counter.exchange(0, std::memory_order_release);
    }

    bool EventMutex::lock(u96 original, const u96 other) noexcept
    {
        while (0 == counter.compare_exchange_weak(original, other, std::memory_order_acquire));

        if (other != counter.load(std::memory_order_acquire))
        {
            return false;
        }
        else
        {
            counter.wait(original, std::memory_order_relaxed);
        }
        return true;
    }

    bool EventMutex::unlock() noexcept
    {
        if (1 == counter.load(std::memory_order_acquire))
        {
            return false;
        }

        if (0 != counter.exchange(1, std::memory_order_release))
        {
            return false;
        }
        else
        {
            counter.notify_one();
        }
        return true;
    }


    EventSemaphore::EventSemaphore() : semaphore(0)
    {

    }

    void EventSemaphore::lock() noexcept
    {
        semaphore.acquire();
    }

    void EventSemaphore::unlock() noexcept
    {
        semaphore.release();
    }
}
