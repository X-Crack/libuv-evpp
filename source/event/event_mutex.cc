﻿#include <event_mutex.h>

namespace Evpp
{
    EventMutex::EventMutex() : counter(ATOMIC_VAR_INIT(0))
    {

    }

    bool EventMutex::try_lock() noexcept
    {
        std::atomic_thread_fence(std::memory_order_acquire);
        return 0 == counter.load(std::memory_order_relaxed);
    }

    bool EventMutex::try_unlock() noexcept
    {
        std::atomic_thread_fence(std::memory_order_release);
        return 0 == counter.exchange(1, std::memory_order_acquire);
    }

    bool EventMutex::lock() noexcept
    {
        if (0 == counter.exchange(1, std::memory_order_acquire))
        {
            counter.wait(1, std::memory_order_relaxed);
            return true;
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

    void EventSemaphore::lock()
    {
        semaphore.acquire();
    }

    void EventSemaphore::unlock()
    {
        semaphore.release();
    }
}
