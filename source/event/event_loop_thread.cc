#include <event_status.h>
#include <event_mutex.h>
#include <event_loop.h>
#include <event_share.h>
#include <event_loop_thread.h>
#include <event_coroutine.h>
#include <future>
namespace Evpp
{
    EventLoopThread::EventLoopThread(EventLoop* base, event_loop* loop, const u96 index) :
        event_base(base),
        event_index(index),
        loop(std::make_shared<EventLoop>(loop, index))
    {

    }

    EventLoopThread::EventLoopThread(EventLoop* base, const std::shared_ptr<EventShare>& share, const u96 index) :
        event_base(base),
        event_index(index),
        loop(std::make_shared<EventLoop>(share->EventLoop(index), index)),
#ifndef EVPP_USE_STL_THREAD
        loop_thread(std::make_unique<event_thread>()),
#endif
        loop_mutex(std::make_unique<EventMutex>())
    {

    }

    EventLoopThread::~EventLoopThread()
    {
        EVENT_INFO("Release Class EventLoopThreadEx");
    }

    bool EventLoopThread::CreaterThread()
    {
        if (ExistsNoneed() && nullptr != event_base)
        {
            if (event_base->EventThread())
            {
#ifdef EVPP_USE_STL_THREAD
                loop_thread.reset(new std::thread(std::bind(&EventLoopThread::WatcherCoroutineInThread, this)));
#else
                if (0 == uv_thread_create(loop_thread.get(), &EventLoopThread::WatcherCoroutineInThread, this))
#endif
                {
                    std::unique_lock<std::mutex> lock(cv_mutex);
                    {
                        if (ChangeStatus(Status::None, Status::Init))
                        {
                            // 启动线程需要慢启动，因为初始化数据过多，否则会导致RunInLoop异步安全初始化失败。
                            if (cv_signal.wait_for(lock, std::chrono::milliseconds(0), std::bind(&EventLoopThread::AvailableEvent, this)))
                            {
                                return true;
                            }
                            else
                            {
                                EVENT_INFO("thread failed to start please wait a little longer");
                            }
                        }
                        assert(0);
                    }
                }
                return false;
            }
            return event_base->RunInLoop(std::bind(&EventLoopThread::CreaterThread, this));
        }
        return false;
    }

    bool EventLoopThread::DestroyThread()
    {
        if (nullptr == loop)
        {
            return false;
        }

        if (ExistsRuning())
        {
            assert(0 == loop->EventThread());

            if (EventLoopAlive(loop->EventBasic()))
            {
                if (loop->StopDispatchEx())
                {
                    if (loop_mutex->try_unlock())
                    {
                        return Join();
                    }
                    return false;
                }
            }
        }
        return false;
    }

    EventLoop* EventLoopThread::GetEventLoop()
    {
        if (nullptr != loop)
        {
            return loop.get();
        }

        if (nullptr != event_base)
        {
            return event_base;
        }

        return nullptr;
    }

    void EventLoopThread::CoroutineInThread()
    {
        if (nullptr == loop)
        {
            return;
        }

        if (ChangeStatus(Status::Init, Status::Exec))
        {
            if (loop->InitialEvent())
            {
                for (; loop_mutex->try_lock();)
                {
#ifdef EVPP_USE_STL_COROUTINES
                    try
                    {
                        if (JoinInTaskEx(std::bind(&EventLoopThread::CoroutineDispatch, this)).get())
                        {
                            if (0 == EventLoopAlive(loop->EventBasic()))
                            {
                                continue;
                            }
                        }
                    }
                    catch (...)
                    {
                        EVENT_INFO("during the operation of the coroutine there may be some problems please check carefully");
                        break;
                    }
#else
                    if (CoroutineDispatch())
                    {
                        break;
                    }
#endif
                }

                if (ChangeStatus(Status::Exec, Status::Stop))
                {
                    return;
                }
            }
            return;
        }
        assert(0);
    }

    bool EventLoopThread::CoroutineDispatch()
    {
        if (nullptr != loop)
        {
            return loop->ExecDispatch(UV_RUN_ONCE);
        }
        return false;
    }

    bool EventLoopThread::AvailableEvent()
    {
        if (nullptr == loop)
        {
            return false;
        }

        while (1)
        {
            if (ExistsRuning())
            {
                if (EventLoopAlive(loop->EventBasic()))
                {
                    cv_signal.notify_one();
                    return true;
                }
            }
            EVPP_THREAD_YIELD();
        }

        return false;
    }

    bool EventLoopThread::Join()
    {
#ifdef EVPP_USE_STL_THREAD
        if (loop_thread && loop_thread->joinable())
        {
            loop_thread->join();
            return true;
        }
        return false;
#else
        return 0 == uv_thread_join(loop_thread.get());
#endif
    }

    void EventLoopThread::WatcherCoroutineInThread(void* handler)
    {
        if (nullptr != handler)
        {
            EventLoopThread* watcher = static_cast<EventLoopThread*>(handler);
            {
                if (nullptr != watcher)
                {
                    watcher->CoroutineInThread();
                }
            }
        }
    }
}
