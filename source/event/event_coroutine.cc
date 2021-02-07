#include <event_coroutine.h>
namespace Evpp
{
#if defined(EVPP_USE_STL_COROUTINES)
    EventCoroutine JoinInTask(std::function<void()> callback)
    {
        co_await EventCoroutineTask(callback);
        co_return true;
    }

    EventCoroutine JoinInTaskEx(std::function<bool()> callback)
    {
        bool value = co_await EventCoroutineTaskEx(callback);

        co_return value;
    }
#endif
}
