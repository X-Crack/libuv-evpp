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
}
#endif // __event_mutex_H__
