#include <event_loop_thread_pool.h>
#include <event_loop.h>
#include <event_share.h>
#include <event_loop_thread.h>
namespace Evpp
{
    EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop, const std::shared_ptr<EventShare>& share) : EventLoopThreadPool(loop, share, 0)
    {

    }

    EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop, const std::shared_ptr<EventShare>& share, const u96 size) :
        event_base(loop),
        event_share(share),
        event_size(size)
    {

    }

    EventLoopThreadPool::~EventLoopThreadPool()
    {

    }

    bool EventLoopThreadPool::CreaterEventThreadPool()
    {
        return CreaterEventThreadPool(event_size);
    }

    bool  EventLoopThreadPool::CreaterEventThreadPool(const u96 size)
    {
        if (nullptr != event_base && event_pool.empty())
        {
            if (event_base->EventThread())
            {
                for (u96 i = 0; i < size; ++i)
                {
                    if (CreaterEventThread(i))
                    {
                        continue;
                    }
                    else
                    {
                        return false;
                    }
                }
                return InitialEventThreadPool(size);
            }
            return event_base->RunInLoop(std::bind<bool(EventLoopThreadPool::*)(const u96)>(&EventLoopThreadPool::CreaterEventThreadPool, this, size));
        }
        return false;
    }

    bool EventLoopThreadPool::DestroyEventThreadPool()
    {
        std::unique_lock<std::recursive_mutex> lock(event_pool_lock);
        {
            for (const auto & [index, var] : event_pool)
            {
                if (var->DestroyThread())
                {
                    continue;
                }
            }

            event_pool.clear();
        }
        return true;
    }

    bool EventLoopThreadPool::CreaterEventThread(const u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(event_pool_lock);
        return event_pool.emplace(index, std::make_unique<EventLoopThread>(event_base, event_share, index)).second;
    }

    bool EventLoopThreadPool::DestroyEventThread(const u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(event_pool_lock);
        {
            if (event_pool[index]->DestroyThread())
            {
                event_pool.erase(index);
                return true;
            }
        }
        return false;
    }

    bool EventLoopThreadPool::InitialEventThread(const u96 index)
    {
        return event_pool[index]->CreaterThread();
    }

    bool EventLoopThreadPool::InitialEventThreadPool(const u96 size)
    {
        for (u96 i = 0; i < size; ++i)
        {
            if (InitialEventThread(i))
            {
                continue;
            }
        }
        return true;
    }

    EventLoop* EventLoopThreadPool::GetEventLoop()
    {
        if (event_pool.empty())
        {
            return event_base;
        }
        return GetEventLoop(event_loop_thread_next.fetch_add(1));
    }

    EventLoop* EventLoopThreadPool::GetEventLoop(u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(event_pool_lock);
        {
            if (event_pool.empty())
            {
                return event_base;
            }

            if (event_pool.size())
            {
                return event_pool[index % event_pool.size()]->GetEventLoop();
            }
        }
        return nullptr;
    }

    EventLoop* EventLoopThreadPool::GetEventLoopEx(event_loop* loop)
    {
        std::unique_lock<std::recursive_mutex> lock(event_pool_lock);
        {
            for (const auto & [index, base] : event_pool)
            {
                if (loop == base->GetEventLoop()->EventBasic())
                {
                    return base->GetEventLoop();
                }
            }
        }
        return nullptr;
    }

    EventLoop* EventLoopThreadPool::GetEventLoopEx(const u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(event_pool_lock);
        {
            if (event_pool.empty())
            {
                return event_base;
            }

            if (event_pool.size())
            {
                return event_pool[index]->GetEventLoop();
            }
        }
        return nullptr;
    }

    std::unique_ptr<EventLoopThread>& EventLoopThreadPool::GetEventLoopThread(const u96 index)
    {
        std::unique_lock<std::recursive_mutex> lock(event_pool_lock);
        return event_pool[index];
    }
}
