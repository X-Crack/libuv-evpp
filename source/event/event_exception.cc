#include <event_exception.h>

namespace Evpp
{
    EventException::EventException(const std::string& msg) : event_exception_message(msg)
    {

    }

    EventException::EventException(std::string&& msg) : event_exception_message(std::move(msg))
    {

    }

    EventException::EventException(const String* msg) : event_exception_message(msg)
    {

    }

    EventException::~EventException()
    {

    }

    const char * EventException::what() noexcept
    {
        return event_exception_message.c_str();
    }

    const char * EventException::what() const noexcept
    {
        return event_exception_message.c_str();
    }


    EventRuntimeException::EventRuntimeException(const std::string& msg, bool var) : EventException(msg), event_exception_value(var)
    {

    }

    EventRuntimeException::EventRuntimeException(std::string&& msg, bool var) : EventException(std::move(msg)), event_exception_value(var)
    {

    }

    EventRuntimeException::EventRuntimeException(const String* msg, bool var) : EventException(msg), event_exception_value(var)
    {

    }

    EventRuntimeException::~EventRuntimeException()
    {

    }

    const bool EventRuntimeException::value() noexcept
    {
        return event_exception_value;
    }

    const bool EventRuntimeException::value() const noexcept
    {
        return event_exception_value;
    }
}