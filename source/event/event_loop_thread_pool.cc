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

    bool EventLoopThreadPool::CreaterEventThreadPool(const u96 size)
    {
        if (event_base && event_pool.empty())
        {
            if (event_base->EventThread())
            {
                for (u96 i = 0; i < size; ++i)
                {
                    std::unique_lock<std::mutex> lock(event_pool_lock);
                    {
                        event_pool.emplace(i, std::make_unique<EventLoopThread>(event_base, event_share, i));
                    }
                }
                return true;
            }
            return event_base->RunInLoop(std::bind((bool(EventLoopThreadPool::*)(const u96)) & EventLoopThreadPool::CreaterEventThreadPool, this, size));
        }
        return false;
    }

    bool EventLoopThreadPool::InitialEventThreadPool()
    {
        if (event_base && event_pool.size())
        {
            if (event_base->EventThread())
            {
                for (u96 i = 0; i < event_pool.size(); ++i)
                {
                    if (false == event_pool[i]->CreaterThread(false))
                    {
                        printf("创建线程失败\n");
                    }
                }
                return true;
            }
            return event_base->RunInLoop(std::bind(&EventLoopThreadPool::InitialEventThreadPool, this));
        }
        return false;
    }

    bool EventLoopThreadPool::SetThreadMaxNum(const u32 size)
    {
        return true;
    }

    u32 EventLoopThreadPool::MemoryUsageSize()
    {
        return (event_pool.size() * sizeof(EventLoopThread)) * 1024;
    }

    EventLoop* EventLoopThreadPool::GetEventLoop()
    {
        if (event_pool.empty())
        {
            return event_base;
        }
        return GetEventLoop(event_loop_thread_next.fetch_add(1));
    }

    EventLoop* EventLoopThreadPool::GetEventLoop(const u96 index)
    {
        std::unique_lock<std::mutex> lock(event_pool_lock);
        {
            if (event_pool.empty())
            {
                return event_base;
            }
            return event_pool[index % event_pool.size()]->GetEventLoop();
        }
    }

    std::unique_ptr<EventLoopThread>& EventLoopThreadPool::GetEventLoopThread(const u96 index)
    {
        std::unique_lock<std::mutex> lock(event_pool_lock);
        return event_pool[index];
    }
}