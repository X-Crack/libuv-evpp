#ifndef __EVENT_DURATION_H__
#define __EVENT_DURATION_H__
#include <event_platform.h>
#include <event_stdint.h>
#include <chrono>
#include <thread>
namespace std
{
    namespace chrono
    {
        namespace detail
        {
            template<typename From, typename To>
            struct posix_duration_cast;

            // chrono -> timeval caster
            template<typename Rep, typename Period>
            struct posix_duration_cast<std::chrono::duration<Rep, Period>, struct timeval>
            {
                static struct timeval cast(std::chrono::duration<Rep, Period> const& d)
                {
                    struct timeval tv;
                    std::chrono::seconds const sec = std::chrono::duration_cast<std::chrono::seconds>(d);
                    tv.tv_sec = sec.count();
                    tv.tv_usec = std::chrono::duration_cast<std::chrono::microseconds>(d - sec).count();
                    return std::move(tv);
                }
            };

            // timeval -> chrono caster
            template<typename Rep, typename Period>
            struct posix_duration_cast<struct timeval, std::chrono::duration<Rep, Period>>
            {
                static std::chrono::duration<Rep, Period> cast(struct timeval const& tv)
                {
                    return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(std::chrono::seconds(tv.tv_sec) + std::chrono::microseconds(tv.tv_usec));
                }
            };
        }

        // chrono -> timeval
        template<typename T, typename Rep, typename Period>
        auto duration_cast(std::chrono::duration<Rep, Period> const& d)
            -> std::enable_if_t<std::is_same<T, struct timeval>::value, struct timeval>
        {
            return detail::posix_duration_cast<std::chrono::duration<Rep, Period>, timeval>::cast(d);
        }

        // timeval -> chrono
        template<typename Duration>
        Duration duration_cast(struct timeval const& tv)
        {
            return detail::posix_duration_cast<struct timeval, Duration>::cast(tv);
        }
    }
}

namespace Evpp
{
    using milliseconds = std::chrono::duration<double, std::ratio<1, 1000>>;
    using microseconds = std::chrono::duration<double, std::ratio<1, 10000>>;

    static void duration_sleep_until(const double v)
    {
        std::this_thread::sleep_for(milliseconds(v));
    }

    static bool CheckSystemClock(const milliseconds& v, const double t)
    {
        return std::chrono::duration_cast<milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch() - v).count() < t;
    }

    static u64 ComplexUnixTimer()
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    class ComputeDuration
    {
    public:
        explicit ComputeDuration(const std::string& name)
            : m_name(name)
            , m_time(std::chrono::duration_cast<milliseconds>(std::chrono::system_clock::now().time_since_epoch()))
        {

        }

        virtual ~ComputeDuration()
        {
            if (m_name.empty())
            {
                EVENT_INFO("Runtime: %f MS", (std::chrono::duration_cast<milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - m_time).count());
            }
            else
            {
                EVENT_INFO("%s -> Runtime: %f MS", m_name.c_str(), (std::chrono::duration_cast<milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - m_time).count());
            }
        }
    private:
        std::string                                 m_name;
        milliseconds                                m_time;
    };

}
#   define EVENT_COMPUTE_DURATION(x) ComputeDuration t(#x)
#endif // __EVENT_DURATION_H__
