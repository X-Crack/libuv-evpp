#ifndef __EVENT_STATUS_H__
#define __EVENT_STATUS_H__
#include <type.h>
#include <atomic>
namespace evpp
{
    class EventStatus
    {
    public:
        enum Status
        {
            NOTYET = 0 << 0,
            INITIALIZING = 1 << 1,
            INITIALIZED = 2 << 2,
            STARTING = 3 << 3,
            RUNNING = 4 << 4,
            PAUSE = 5 << 5,
            STOPPED = 6 << 6,
            DESTROYED = 7 << 7
        };
    public:
        explicit EventStatus();
        virtual ~EventStatus();
    public:
        bool ChangeStatus(const Status original, const Status other);
        bool ExistsStarts(const Status other);
    public:
        bool ExistsRuning();
        bool ExistsStoped();
    private:
        std::atomic<Status>                                      status;
    };
}
#endif // __event_status_H__