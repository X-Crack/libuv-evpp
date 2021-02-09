﻿#include <event_mutex.h>

namespace Evpp
{
    EventMutex::EventMutex() : counter(ATOMIC_VAR_INIT(0))
    {

    }

    bool EventMutex::try_lock() noexcept
    {
        return 0 == counter.load(std::memory_order_relaxed);
    }

    bool EventMutex::try_unlock() noexcept
    {
        return 0 == counter.exchange(1, std::memory_order_acquire);
    }

    bool EventMutex::lock(u96 original, const u96 other) noexcept
    {
        while (0 == counter.compare_exchange_weak(original, other, std::memory_order_acquire));

        if (other == counter.load(std::memory_order_acquire))
        {
            counter.wait(other, std::memory_order_relaxed);
            {
                return true;
            }
        }
        return false;
    }

    bool EventMutex::unlock() noexcept
    {
        if (counter.load(std::memory_order_acquire))
        {
            counter.store(0, std::memory_order_release);
            counter.notify_one();
            return true;
        }
        return false;
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
