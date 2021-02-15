#ifndef __EVENT_EXCEPTION_H__
#define __EVENT_EXCEPTION_H__
#include <event_config.h>
#include <functional>
#include <exception>
#include <string>
#include <source_location.hpp>
namespace Evpp
{
    class EVPP_EXPORT EventException 
    {
    public:
        explicit EventException(const std::string& msg);
        explicit EventException(std::string&& msg);
        explicit EventException(const String* msg);
        virtual ~EventException();
    public:
        const char * what() noexcept;
        virtual const char* what() const noexcept;
    private:
        std::string                                                             event_exception_message;
    };

    /// template???
    class EVPP_EXPORT EventRuntimeException final
    {
    public:
        explicit EventRuntimeException(const String* msg, const Handler& handler, bool var, const std::source_location& location = std::source_location::current());
        explicit EventRuntimeException(const std::string& msg, const Handler& handler, bool var, const std::source_location& location);
        virtual ~EventRuntimeException();
    public:
        const char * what() noexcept;
        virtual const char* what() const noexcept;
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
    public:
        constexpr const char* file_name() noexcept;
        constexpr const char* function_name() noexcept;
        constexpr const u32 line() noexcept;
        constexpr const u32 column() noexcept;
    public:
        constexpr const char* file_name() const noexcept;
        constexpr const char* function_name() const noexcept;
        constexpr const u32 line() const noexcept;
        constexpr const u32 column() const noexcept;
    private:
        std::string                                                             event_exception_message;
        Handler                                                                 event_exception_handler;
        bool                                                                    event_exception_value;
        std::source_location                                                    event_exception_location;
        // helper https://zh.cppreference.com/w/cpp/header/source_location
        // https://docs.microsoft.com/en-us/cpp/overview/visual-cpp-language-conformance?view=msvc-160
        // P1208R6 <source_location>        No
        // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1208r6.pdf
    private:
        EventRuntimeException(const EventRuntimeException&) = default;
        EventRuntimeException(EventRuntimeException&&) = default;
    };
}
#endif // __EVENT_EXCEPTION_H__