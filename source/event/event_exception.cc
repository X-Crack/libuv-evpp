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


    EventRuntimeException::EventRuntimeException(const String* msg, const Handler& handler, bool var, const std::source_location& location) :
        EventRuntimeException(std::string(msg), handler, var, location)
    {

    }

    EventRuntimeException::EventRuntimeException(const std::string& msg, const Handler& handler, bool var, const std::source_location& location) :
        event_exception_message(msg),
        event_exception_handler(handler),
        event_exception_value(var),
        event_exception_location(location)
    {
        
    }

    EventRuntimeException::~EventRuntimeException()
    {
        
    }

    const char * EventRuntimeException::what() noexcept
    {
        return event_exception_message.c_str();
    }

    const char* EventRuntimeException::what() const noexcept
    {
        return event_exception_message.c_str();
    }

    const bool EventRuntimeException::value() noexcept
    {
        return event_exception_value;
    }

    const bool EventRuntimeException::value() const noexcept
    {
        return event_exception_value;
    }

    constexpr const char* EventRuntimeException::file_name() noexcept
    {
        return event_exception_location.file_name();
    }

    constexpr const char* EventRuntimeException::function_name() noexcept
    {
        return event_exception_location.function_name();
    }

    constexpr const u32 EventRuntimeException::line() noexcept
    {
        return event_exception_location.line();
    }

    constexpr const u32 EventRuntimeException::column() noexcept
    {
        return event_exception_location.column();
    }

    constexpr const char* EventRuntimeException::file_name() const noexcept
    {
        return event_exception_location.file_name();
    }

    constexpr const char* EventRuntimeException::function_name() const noexcept
    {
        return event_exception_location.function_name();
    }

    constexpr const u32 EventRuntimeException::line() const noexcept
    {
        return event_exception_location.line();
    }

    constexpr const u32 EventRuntimeException::column() const noexcept
    {
        return event_exception_location.column();
    }
}