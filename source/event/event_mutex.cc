#include <event_mutex.h>
namespace Evpp
{
    EventSemaphore::EventSemaphore() : event_cv_pending(1)
    {

    }

    EventSemaphore::~EventSemaphore()
    {

    }

    bool EventSemaphore::StarWaiting(const std::chrono::milliseconds& delay)
    {
        for (auto time = std::chrono::steady_clock::now(); std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time).count() < delay.count();)
        {
            if (1 == event_cv_pending.exchange(0))
            {
                return true;
            }
            else
            {
                event_cv_pending.wait(0, std::memory_order_relaxed);
            }
        }
        return true;
    }

    bool EventSemaphore::StopWaiting()
    {
        event_cv_pending.store(1);
        event_cv_pending.notify_one();
        return true;
    }
}
