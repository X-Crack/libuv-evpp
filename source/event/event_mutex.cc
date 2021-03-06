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
}
