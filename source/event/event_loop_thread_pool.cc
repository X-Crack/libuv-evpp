#include <event_loop_thread_pool.h>
#include <event_loop.h>
#include <event_share.h>
#include <event_loop_thread.h>
#include <event_loop_thread_ex.h>
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

    bool EventLoopThreadPool::CreaterEventThreadPool(const u96 size, const bool use_thread_ex)
    {
        if (nullptr != event_base && (event_pool.empty() && event_pool_ex.empty()))
        {
            if (event_base->EventThread())
            {
                for (u96 i = 0; i < size; ++i)
                {
                    std::unique_lock<std::mutex> lock(event_pool_lock);
                    {
                        if (use_thread_ex)
                        {
                            event_pool_ex.emplace(i, std::make_unique<EventLoopThreadEx>(event_base, event_share, i));
                        }
                        else
                        {
                            event_pool.emplace(i, std::make_unique<EventLoopThread>(event_base, event_share, i));
                        }
                    }
                }
                return InitialEventThreadPool(size, use_thread_ex);
            }
            return event_base->RunInLoop(std::bind((bool(EventLoopThreadPool::*)(const u96, const bool))&EventLoopThreadPool::CreaterEventThreadPool, this, size, use_thread_ex));
        }
        return false;
    }

    bool EventLoopThreadPool::InitialEventThreadPool(const u96 size, const bool use_thread_ex)
    {
        for (u96 i = 0; i < size; ++i)
        {
            if (use_thread_ex)
            {
                if (event_pool_ex[i]->CreaterSubThread())
                {
                    continue;
                }

                return false;
            }

            if (event_pool[i]->CreaterSubThread())
            {
                continue;
            }

            return false;
        }
        return true;
    }

    bool EventLoopThreadPool::SetThreadMaxNum(const u32 size)
    {
        return true;
    }

    EventLoop* EventLoopThreadPool::GetEventLoop()
    {
        if (event_pool.empty() && event_pool_ex.empty())
        {
            return event_base;
        }
        return GetEventLoop(event_loop_thread_next.fetch_add(1));
    }

    EventLoop* EventLoopThreadPool::GetEventLoop(const u96 index)
    {
        std::unique_lock<std::mutex> lock(event_pool_lock);
        {
            if (event_pool.empty() && event_pool_ex.empty())
            {
                return event_base;
            }

            if (event_pool.size())
            {
                return event_pool[index % event_pool.size()]->GetEventLoop();
            }

            if (event_pool_ex.size())
            {
                return event_pool_ex[index % event_pool_ex.size()]->GetEventLoop();
            }
        }
        return nullptr;
    }

    std::unique_ptr<EventLoopThread>& EventLoopThreadPool::GetEventLoopThread(const u96 index)
    {
        std::unique_lock<std::mutex> lock(event_pool_lock);
        return event_pool[index];
    }
}