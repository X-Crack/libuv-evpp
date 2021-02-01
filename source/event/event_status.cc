#include <event_status.h>
namespace Evpp
{
    EventStatus::EventStatus() : status(Status::None)
    {

    }

    EventStatus::~EventStatus()
    {
        LOG_INFO << "Release Class EventStatus";
    }

    bool EventStatus::ChangeStatus(Status original, const Status other)
    {
        return status.compare_exchange_strong(original, other, std::memory_order_release);
    }

    bool EventStatus::ChangeStatus(const Status other)
    {
        return Status::None != status.exchange(other, std::memory_order_release);
    }

    bool EventStatus::ExistsStarts(const Status other)
    {
        return other == status;
    }

    bool EventStatus::ExistsNoneed()
    {
        return Status::None == status;
    }

    bool EventStatus::ExistsInited()
    {
        return Status::Init == status;
    }

    bool EventStatus::ExistsRuning()
    {
        return Status::Exec == status;
    }

    bool EventStatus::ExistsStoped()
    {
        return Status::Stop == status;
    }

    bool EventStatus::ExistsExited()
    {
        return Status::Exit == status;
    }
}