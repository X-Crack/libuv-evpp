#include <event_coroutine.h>
namespace Evpp
{
#if defined(EVPP_USE_STL_COROUTINES)
    EventCoroutine JoinInTask(Handler&& callback)
    {
        co_await EventCoroutineTask(std::move(callback));
        co_return true;
    }

    EventCoroutine JoinInTaskEx(Functor&& callback)
    {
        bool value = co_await EventCoroutineTaskEx(std::move(callback));

        co_return value;
    }
#endif
}
