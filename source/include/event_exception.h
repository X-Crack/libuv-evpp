#ifndef __EVENT_EXCEPTION_H__
#define __EVENT_EXCEPTION_H__
#include <event_config.h>
#include <functional>
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

    /// template???
    class EventRuntimeException final : public EventException
    {
    public:
        explicit EventRuntimeException(const std::string& msg, const Handler& handler, bool var);
        explicit EventRuntimeException(std::string&& msg, Handler&& handler, bool var);
        explicit EventRuntimeException(const String* msg, const Handler& handler, bool var);
        virtual ~EventRuntimeException();
    public:
        const bool value() noexcept;
        const bool value() const noexcept;
    public:
        template <class _Ty>
        const _Ty* handler() noexcept
        {
            return event_exception_handler.target<_Ty>();
        }

        template <class _Ty>
        const _Ty* handler() const noexcept
        {
            return event_exception_handler.target<_Ty>();
        }

        template <class _Ty>
        const bool handler(const Handler& other) noexcept
        {
            return *event_exception_handler.target<_Ty>() == *other.target<_Ty>();
        }

        template <class _Ty>
        const bool handler(const Handler& other) const noexcept
        {
            return *event_exception_handler.target<_Ty>() == *other.target<_Ty>();
        }
    private:
        /// template???
        Handler                                                                 event_exception_handler;
        bool                                                                    event_exception_value;
    };
}
#endif // __EVENT_EXCEPTION_H__