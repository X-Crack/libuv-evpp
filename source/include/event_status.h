#ifndef __EVENT_STATUS_H__
#define __EVENT_STATUS_H__
#include <type.h>
#include <atomic>
namespace Evpp
{
    class EventStatus
    {
    public:
        enum class Status : u32
        {
            None = 1 << 0,
            Init = 1 << 1,
            Exec = 1 << 2,
            Stop = 1 << 3,
        };
    public:
        explicit EventStatus();
        virtual ~EventStatus();
    public:
        bool ChangeStatus(Status original = Status::None, const Status other = Status::None);
        bool ExistsStarts(const Status other);
    public:
        bool ExistsRuning();
        bool ExistsStoped();
    private:
        std::atomic<Status>                                      status;
    };
}
#endif // __event_status_H__