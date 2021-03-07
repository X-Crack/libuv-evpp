#include <event_mutex.h>
namespace Evpp
{
    EventMutex::EventMutex(event_mutex* lock) :
        event_lock_mutex(lock)
    {
        uv_mutex_lock(event_lock_mutex);
    }

    EventMutex::~EventMutex()
    {
        uv_mutex_unlock(event_lock_mutex);
    }

    EventLocking::EventLocking() : event_cv_flag(0)
    {

    }

    EventLocking::~EventLocking()
    {

    }

    bool EventLocking::dowait(const std::chrono::milliseconds& delay)
    {
        std::unique_lock<std::mutex> lock(event_cv_mutex);
        {
            if (event_cv_signal.wait_until(lock, std::chrono::system_clock::now() + delay, std::bind(&EventLocking::advise, this)))
            {
                return true;
            }
        }
        return false;
    }

    bool EventLocking::notify()
    {
        std::unique_lock<std::mutex> lock(event_cv_mutex);
        {
            if (0 == event_cv_flag.exchange(1))
            {
                event_cv_signal.notify_one();
            }

            if (event_cv_flag.load())
            {
                return true;
            }
        }
        return false;
    }

    bool EventLocking::advise()
    {
        if (event_cv_flag.load())
        {
            return true;
        }
        return false;
    }
}
