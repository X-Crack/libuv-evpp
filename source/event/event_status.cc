#include <event_status.h>
namespace evpp
{
    EventStatus::EventStatus() : status(NOTYET)
    {

    }

    EventStatus::~EventStatus()
    {

    }

    bool EventStatus::ChangeStatus(const Status original, const Status other)
    {
        return original == status.exchange(other, std::memory_order_release);
    }

    bool EventStatus::ExistsStarts(const Status other)
    {
        return other == status;
    }

    bool EventStatus::ExistsRuning()
    {
        return RUNNING == status;
    }

    bool EventStatus::ExistsStoped()
    {
        return STOPPED == status;
    }
}