#include <event_coroutine.h>
namespace Evpp
{
#if defined(EVPP_USE_STL_COROUTINES)
    EventCoroutine EventCoroutine::JoinInTask(const std::function<void()>& callback)
    {
        const bool var = co_await EventCoroutineTask(callback);
    }

    EventCoroutine EventCoroutine::JoinInTaskEx(const std::function<bool()>& callback)
    {
        const bool var = co_await EventCoroutineTaskEx(callback);
    }
#endif
}
