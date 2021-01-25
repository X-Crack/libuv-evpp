#ifndef __EVENT_LOOP_THREAD_POOL_H__
#define __EVENT_LOOP_THREAD_POOL_H__
#include <config.h>
#include <memory>
#include <mutex>
#include <atomic>
namespace Evpp
{
    class EventLoop;
    class EventShare;
    class EventLoopThread;
    class EventLoopThreadPool
    {
    public:
        explicit EventLoopThreadPool(EventLoop* loop, const std::shared_ptr<EventShare>& share);
        explicit EventLoopThreadPool(EventLoop* loop, const std::shared_ptr<EventShare>& share, const u96 size);
        virtual ~EventLoopThreadPool();
    public:
        bool CreaterEventThreadPool();
        bool CreaterEventThreadPool(const u96 size);
        bool InitialEventThreadPool();
    public:
        u96  GetEventThreadLoopSize() { return event_pool.size(); };
    public:
        u32  MemoryUsageSize();
    public:
        EventLoop* GetEventLoop();
        EventLoop* GetEventLoop(const u96 index);
        std::unique_ptr<EventLoopThread>& GetEventLoopThread(const u96 index);
    private:
        EventLoop*                                                                              event_base;
        std::shared_ptr<EventShare>                                                             event_share;
        u96                                                                                     event_size;
        std::unordered_map<u96, std::unique_ptr<EventLoopThread>>                               event_pool;
        // 线程池高级实现，逐渐开发。
        std::atomic<u32>                                                                        event_pool_min;         // 最小线程
        std::atomic<u32>                                                                        event_pool_max;         // 最大线程
        std::atomic<u32>                                                                        event_pool_let;         // 空闲线程
        std::mutex										                                        event_pool_lock;

        std::atomic<u96>															            event_loop_thread_next;
    };
}
#endif // __EVENT_LOOP_THREAD_POOL_H__