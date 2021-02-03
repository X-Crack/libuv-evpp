#include <event_coroutine.h>
namespace Evpp
{
    EventCoroutine EventCoroutine::JoinInTask(const std::function<bool()>& callback)
    {
        const bool ret = co_await EventCoroutineTask(callback);
    }
}
