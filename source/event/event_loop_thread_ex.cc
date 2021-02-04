#include <event_status.h>
#include <event_loop.h>
#include <event_loop_thread_ex.h>
#include <event_share.h>
#include <event_coroutine.h>
namespace Evpp
{
#ifdef __cpp_coroutines
    EventLoopThreadEx::EventLoopThreadEx(EventLoop* base, event_loop* loop, const u96 index) :
        event_base(base),
        event_index(index),
        loop(std::make_shared<EventLoop>(loop, index)),
        loop_exit(1)
    {

    }

    EventLoopThreadEx::EventLoopThreadEx(EventLoop* base, const std::shared_ptr<EventShare>& share, const u96 index) :
        event_base(base),
        event_share(share),
        event_index(index),
        loop(std::make_shared<EventLoop>(event_share->EventLoop(index), index)),
        loop_exit(1)
    {

    }

    EventLoopThreadEx::~EventLoopThreadEx()
    {
        if (Join())
        {
            EVENT_INFO("Release Class EventLoopThreadEx");
        }
    }

    bool EventLoopThreadEx::CreaterSubThread()
    {
        if (nullptr == loop_thread && event_base)
        {
            if (event_base->EventThread())
            {
                loop_thread.reset(new std::thread(std::bind(&EventLoopThreadEx::CoroutineInThread, this)));
                {
                    std::unique_lock<std::mutex> lock(cv_mutex);
                    {
                        if (ChangeStatus(Status::None, Status::Init))
                        {
                            // 启动线程需要慢启动，因为初始化数据过多，否则会导致RunInLoop异步安全初始化失败。
                            if (cv_signal.wait_for(lock, std::chrono::milliseconds(8), std::bind(&EventLoopThreadEx::AvailableEvent, this)))
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
            return event_base->RunInLoop(std::bind(&EventLoopThreadEx::CreaterSubThread, this));
        }
        return false;
    }

    bool EventLoopThreadEx::DestroyThread()
    {
        if (nullptr == loop)
        {
            return false;
        }

        if (ExistsRuning())
        {
            assert(0 == loop->EventThread());

            if (uv_loop_alive(loop->EventBasic()))
            {
                if (StopDispatch())
                {
                    if (loop_exit)
                    {
                        loop_exit.store(0, std::memory_order_release);
                    }
                    return true;
                }
            }
        }
        return false;
    }

    EventLoop* EventLoopThreadEx::GetEventLoop()
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

    bool EventLoopThreadEx::StopDispatch()
    {
        if (nullptr != loop)
        {
            return loop->StopDispatchEx();
        }
        return false;
    }

    void EventLoopThreadEx::CoroutineInThread()
    {
        if (nullptr == loop)
        {
            return;
        }

        if (ChangeStatus(Status::Init, Status::Exec))
        {
            if (loop->InitialEvent())
            {
                while (loop_exit.load(std::memory_order_acquire))
                {
                    if (uv_loop_alive(loop->EventBasic()))
                    {
                        try
                        {
                            EventCoroutine::JoinInTask(std::bind(&EventLoopThreadEx::CoroutineDispatch, this));
                        }
                        catch (...)
                        {
                            EVENT_INFO("during the operation of the coroutine there may be some problems please check carefully");
                            break;
                        }
                    }
                }

                if (0 || UV_EBUSY == uv_loop_close(loop->EventBasic()))
                {
                    EVENT_INFO("delete eventLoop ok %d", event_index);
                }

                if (ChangeStatus(Status::Exec, Status::Stop))
                {
                    assert(loop->ExistsRuning());
                }
            }
        }
    }

    bool EventLoopThreadEx::CoroutineDispatch()
    {
        if (nullptr != loop)
        {
            if (uv_loop_alive(loop->EventBasic()))
            {
                return loop->ExecDispatch(UV_RUN_NOWAIT);
            }
        }
        return false;
    }

    bool EventLoopThreadEx::AvailableEvent()
    {
        if (nullptr == loop)
        {
            return false;
        }

        if (ExistsRuning())
        {
            return 0 == loop->EventBasic()->stop_flag;
        }

        return false;
    }

    bool EventLoopThreadEx::Join()
    {
        if (loop_thread && loop_thread->joinable())
        {
            EVENT_INFO("Join：%d", GetCurrentThreadId());
            loop_thread->join();
            return true;
        }
        return false;
    }
#else
    EventLoopThreadEx::EventLoopThreadEx(const u96 index) : event_base(nullptr), event_index(index)
    {

    }

    EventLoopThreadEx::EventLoopThreadEx(EventLoop* loop, const std::shared_ptr<EventShare>& share, const u96 index) : event_base(loop), event_share(share), event_index(index)
    {

    }

    EventLoopThreadEx::~EventLoopThreadEx()
    {
        if (Join())
        {
            loop_thread.reset();
        }
    }

    bool EventLoopThreadEx::CreaterSubThread(bool wait)
    {
        if (nullptr == loop_thread && event_base)
        {
            if (event_base->EventThread())
            {
                loop_thread.reset(new std::thread(std::bind(&EventLoopThreadEx::Run, this)));
                {
                    if (wait)
                    {
                        std::unique_lock<std::mutex> lock(cv_mutex);
                        {
                            if (cv_signal.wait_for(lock, std::chrono::milliseconds(64), std::bind(&EventLoopThreadEx::AvailableEvent, this)))
                            {
                                return this->Join();
                            }
                        }
                    }
                }
                return true;
            }
            return event_base->RunInLoop(std::bind(&EventLoopThreadEx::CreaterSubThread, this, wait));
        }
        return false;
    }

    bool EventLoopThreadEx::Join()
    {
        if (loop_thread && loop_thread->joinable())
        {
            loop_thread->detach();
            return true;
        }
        return false;
    }

    EventLoop* EventLoopThreadEx::GetEventLoop()
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

    bool EventLoopThreadEx::AvailableEvent()
    {
        if (nullptr == loop)
        {
            return false;
        }

        if (ExistsStarts(INITIALIZED))
        {
            return 0 == loop->EventBasic()->stop_flag;
        }

        return false;
    }

    void EventLoopThreadEx::Run()
    {
        if (ChangeStatus(NOTYET, INITIALIZING))
        {
            loop.reset(new EventLoop(event_share->EventLoop(event_index), event_index));
            {
                if (ChangeStatus(INITIALIZING, INITIALIZED))
                {
                    if (loop->InitialEvent())
                    {
                        if (false == loop->ExecDispatch())
                        {
                            assert(0);
                            return;
                        }

                        if (ChangeStatus(INITIALIZED, STOPPED))
                        {
                            assert(loop->ExistsStoped());
                        }
                    }
                }
            }
    }
}
#endif
}
