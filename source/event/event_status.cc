#include <event_status.h>
namespace Evpp
{
    EventStatus::EventStatus() : status(None)
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
        return Exec == status;
    }

    bool EventStatus::ExistsStoped()
    {
        return Stop == status;
    }
}