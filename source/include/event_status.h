#ifndef __EVENT_STATUS_H__
#define __EVENT_STATUS_H__
#include <event_config.h>
#include <atomic>
namespace Evpp
{
    class EventStatus
    {
    public:
        enum class Status : u32
        {
            None = 1 << 1,
            Init = 1 << 2,
            Exec = 1 << 3,
            Stop = 1 << 4,
            Exit = 1 << 5,
        };
    public:
        explicit EventStatus();
        virtual ~EventStatus();
    public:
        bool ChangeStatus(Status original, const Status other);
        bool ChangeStatus(const Status other);
        bool ExistsStarts(const Status other);
    public:
        bool ExistsNoneed();
        bool ExistsInited();
        bool ExistsRuning();
        bool ExistsStoped();
        bool ExistsExited();
    protected:
        std::atomic<Status>                                      status;
    };
}
#endif // __event_status_H__