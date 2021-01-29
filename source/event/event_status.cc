#include <event_status.h>
namespace Evpp
{
    EventStatus::EventStatus() : status(Status::None)
    {

    }

    EventStatus::~EventStatus()
    {

    }

    bool EventStatus::ChangeStatus(Status original, const Status other)
    {
        return status.compare_exchange_strong(original, other, std::memory_order_release);
    }

    bool EventStatus::ExistsStarts(const Status other)
    {
        return other == status;
    }

    bool EventStatus::ExistsRuning()
    {
        return Status::Exec == status;
    }

    bool EventStatus::ExistsStoped()
    {
        return Status::Stop == status;
    }
}