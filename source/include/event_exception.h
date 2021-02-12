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
    class EventRuntimeException final : public std::exception
    {
    public:
        explicit EventRuntimeException(const String* msg, const Handler& handler, bool var);
        explicit EventRuntimeException(const std::string& msg, const Handler& handler, bool var);
        virtual ~EventRuntimeException();
    public:
        const char * what() noexcept;
        virtual const char* what() const noexcept override;
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
            if (nullptr != other && event_exception_handler != nullptr)
            {
                return *event_exception_handler.target<_Ty>() == *other.target<_Ty>();
            }
            return false;
        }

        template <class _Ty>
        const bool handler(const Handler& other) const noexcept
        {
            if (nullptr != other && event_exception_handler != nullptr)
            {
                return *event_exception_handler.target<_Ty>() == *other.target<_Ty>();
            }
            return false;
        }
    private:
        /// template???
        std::string                                                             event_exception_message;
        Handler                                                                 event_exception_handler;
        bool                                                                    event_exception_value;
        // helper https://zh.cppreference.com/w/cpp/header/source_location
        // https://docs.microsoft.com/en-us/cpp/overview/visual-cpp-language-conformance?view=msvc-160
        // P1208R6 <source_location>        No
        // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1208r6.pdf
        // std::source_location                                                    event_exception_location;
    };
}
#endif // __EVENT_EXCEPTION_H__