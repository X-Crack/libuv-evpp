#ifndef __EVENT_EXCEPTION_H__
#define __EVENT_EXCEPTION_H__
#include <event_config.h>
#include <exception>
#include <string>
namespace Evpp
{
    class EventException : public std::exception
    {
    public:
        explicit EventException(const std::string& msg);
        explicit EventException(std::string&& msg);
        explicit EventException(const String* msg);
        virtual ~EventException();
    public:
        const char * what() noexcept;
        virtual const char* what() const noexcept override;
    private:
        std::string                                                             event_exception_message;
    };


    class EventRuntimeException : public EventException
    {
    public:
        explicit EventRuntimeException(const std::string& msg, bool var);
        explicit EventRuntimeException(std::string&& msg, bool var);
        explicit EventRuntimeException(const String* msg, bool var);
        virtual ~EventRuntimeException();
    public:
        const bool value() noexcept;
        const bool value() const noexcept;
    private:
        bool                                                                    event_exception_value;
    };
}
#endif // __EVENT_EXCEPTION_H__