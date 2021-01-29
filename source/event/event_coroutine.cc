#include <event_coroutine.h>
namespace Evpp
{
#ifdef __cpp_coroutines
    EventCoroutine EventCoroutine::JoinInTask(const std::function<bool()>& callback)
    {
       const bool ret = co_await EventCoroutineTask(callback);
    }
#endif
}