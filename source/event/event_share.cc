#include <event_share.h>
#include <event_loop.h>
namespace Evpp
{
    EventShare::EventShare() : event_base(uv_default_loop())
    {

    }

    EventShare::~EventShare()
    {
        if (event_loops.size())
        {
            this->DestroyLoops();
        }
    }

    bool EventShare::CreaterLoops(const u96 size)
    {
        for (u96 i = 0; i < size; ++i)
        {
            std::unique_lock<std::mutex> lock(event_mutex);
            {
                event_loops.emplace(i, new event_loop());
            }
        }
        return nullptr != event_base;
    }

    bool EventShare::DestroyLoops()
    {
        std::unique_lock<std::mutex> lock(event_mutex);
        {
            for (auto & [index, loop] : event_loops)
            {
                if (0 == uv_loop_close(loop))
                {
                    loop = nullptr;
                }
                else
                {
                    return false;
                }
            }

            event_loops.clear();
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
                return event_loops[index % event_loops.size()];
            }
        }
        return nullptr;
    }

    event_loop* EventShare::DefaultEventLoop()
    {
        return event_base;
    }

    event_loop* EventShare::CreaterDefaultEventLoop()
    {
        return new event_loop();
    }
}