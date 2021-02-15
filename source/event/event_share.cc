#include <event_share.h>
#include <event_loop.h>
namespace Evpp
{
    EventShare::EventShare() : event_base(EVENT_UV_GLOBAL == nullptr ? uv_default_loop() : EVENT_UV_GLOBAL.load())
    {
        if (EVENT_UV_GLOBAL == nullptr)
        {
            EVENT_UV_GLOBAL = event_base;
        }
    }

    EventShare::~EventShare()
    {
        if (nullptr != EVENT_UV_GLOBAL)
        {
            EVENT_UV_GLOBAL = nullptr;
        }
#if defined(EVENT_DEBUG_MODE)
        this->DestroyLoops();
#endif
    }

    bool EventShare::CreaterLoops(const u96 size)
    {
        for (u96 i = 0; i < size; ++i)
        {
            std::unique_lock<std::mutex> lock(event_mutex);
            {
                event_loops.emplace(i, std::make_unique<event_loop>());
            }
        }
        return nullptr != event_base;
    }

    bool EventShare::DestroyLoops()
    {
        std::unique_lock<std::mutex> lock(event_mutex);
        {
            for (u96 i = 0; i < event_loops.size(); ++i)
            {
                if (EventLoopAlive(event_loops[i].get()))
                {
                    EVENT_WARN("event loop release has some problems but it is not important");
                    assert(0);
                }
            }
        }
        return true;
    }

    u32 EventShare::GetHardwareThreads()
    {
        return std::thread::hardware_concurrency();
    }

    event_loop* EventShare::EventLoop(const u96 index)
    {
        std::unique_lock<std::mutex> lock(event_mutex);
        {
            if (0 != event_loops.size())
            {
                return event_loops[index % event_loops.size()].get();
            }
        }
        return nullptr;
    }

    event_loop* EventShare::DefaultEventLoop()
    {
        return event_base;
    }
}
