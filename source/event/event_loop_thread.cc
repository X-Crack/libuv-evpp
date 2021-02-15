#include <event_status.h>
#include <event_loop.h>
#include <event_share.h>
#include <event_loop_thread.h>
#include <event_coroutine.h>
#include <event_exception.h>
#include <event_mutex.h>
#include <future>
#ifdef EVPP_USE_BOOST_THREAD
#include <boost/thread/thread.hpp>
#endif
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
        event_semaphore(std::make_unique<EventSemaphore>()),
        loop(std::make_shared<EventLoop>(share->EventLoop(index), index)),
#if defined(EVPP_USE_STL_THREAD)
        loop_thread(std::make_unique <std::thread>())
#elif defined(EVPP_USE_BOOST_THREAD)
        loop_thread(std::make_unique<boost::thread>())
#else
        loop_thread(std::make_unique<event_thread>())
#endif
    {

    }

    EventLoopThread::~EventLoopThread()
    {

    }

    bool EventLoopThread::CreaterThread()
    {
        if (nullptr != event_base && ExistsNoneed())
        {
            if (event_base->EventThread())
            {
                if (ChangeStatus(Status::None, Status::Init))
                {
                    try
                    {
#if defined(EVPP_USE_STL_THREAD)
                        loop_thread.reset(new std::thread(std::bind(&EventLoopThread::WatcherCoroutineInThread, this)));
#elif defined(EVPP_USE_BOOST_THREAD)
                        loop_thread.reset(new boost::thread(std::bind(&EventLoopThread::WatcherCoroutineInThread, this)));
#else
                        if (0 == uv_thread_create(loop_thread.get(), &EventLoopThread::WatcherCoroutineInThread, this))
#endif
                        {
                            std::unique_lock<std::mutex> lock(cv_mutex);
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
                                assert(0);
                            }
                        }
                    }
                    catch (const std::system_error &ex)
                    {
                        EVENT_ERROR("createrthread error coe: %d error message: %s", ex.code().value(), ex.what());
                    }
                }
                return false;
            }
            return event_base->RunInLoopEx(std::bind(&EventLoopThread::CreaterThread, this));
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
            if (loop->StopDispatch())
            {
                return Join();
            }
            return false;
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
                for (; EventLoopAlive(loop->EventBasic());)
                {
#ifdef EVPP_USE_STL_COROUTINES
                    try
                    {
                        if (JoinInTaskEx(std::bind(&EventLoopThread::CoroutineDispatch, this)).get())
                        {
                            continue;
                        }
                    }
                    catch (const EventRuntimeException& ex)
                    {
                        EVENT_INFO("%s %d", ex.what(), ex.value());
                    }
                    catch (const EventException& ex)
                    {
                        EVENT_INFO("during the operation of the coroutine there may be some problems please check carefully : %s", ex.what());
                    }
                    catch (...)
                    {
                        EVENT_INFO("this is a exception without handling");
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
                if (EventLoopAlive(loop->EventBasic()) && loop->ExistsRuning())
                {
                    cv_signal.notify_one();
                    return true;
                }
            }
        }

        return false;
    }

    bool EventLoopThread::Join()
    {
#if defined(EVPP_USE_STL_THREAD) || defined(EVPP_USE_BOOST_THREAD)
        try
        {
            if (loop_thread && loop_thread->joinable())
            {
                loop_thread->join();
                loop_thread.reset();
                return ChangeStatus(Status::Stop, Status::Exit);
            }
        }
        catch (const std::system_error &ex)
        {
            EVENT_ERROR("destroythread error coe: %d error message: %s", ex.code().value(), ex.what());
        }
#else
        if (0 == uv_thread_join(loop_thread.get()))
        {
            return 0 == uv_thread_join(loop_thread.get());
        }
#endif
        return false;
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