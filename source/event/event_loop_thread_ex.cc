#include <event_status.h>
#include <event_loop.h>
#include <event_loop_thread_ex.h>
#include <event_share.h>
namespace Evpp
{
#ifdef __cpp_coroutines
    EventLoopThreadEx::EventLoopThreadEx(const u96 index) : 
        event_base(nullptr), 
        event_index(index)
    {

    }

    EventLoopThreadEx::EventLoopThreadEx(EventLoop* loop, const std::shared_ptr<EventShare>& share, const u96 index) : 
        event_base(loop), 
        event_share(share), 
        event_index(index),
        event_coroutine_task(std::make_unique<EventCoroutineTask>(std::bind(&EventLoopThreadEx::CoroutineDispatch, this))/* new EventCoroutineTask(std::bind(&EventLoopThreadEx::CoroutineDispatch, this))*/),
        loop_exit(0)
    {

    }

    EventLoopThreadEx::~EventLoopThreadEx()
    {
        if (Join())
        {

        }
    }

    bool EventLoopThreadEx::CreaterSubThread(bool wait)
    {
        if (nullptr != event_coroutine_task)
        {
            if (nullptr == loop_thread && event_base)
            {
                if (event_base->EventThread())
                {
                    loop_thread.reset(new std::thread(std::bind(&EventLoopThreadEx::CoroutineInThread, this)));
                    {
                        if (wait)
                        {
                            std::unique_lock<std::mutex> lock(cv_mutex);
                            {
                                if (cv_signal.wait_for(lock, std::chrono::milliseconds(64), std::bind(&EventLoopThreadEx::AvailableEvent, this)))
                                {
                                    return Join();
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
        return false;
    }

    bool EventLoopThreadEx::DestroyThread()
    {
        if (nullptr != loop)
        {
            if (uv_loop_alive(loop->EventBasic()))
            {
                if (0 == loop_exit)
                {
                    loop_exit.store(1);
                }
                return loop->StopDispatch();
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

    void EventLoopThreadEx::CoroutineInThread()
    {
        if (ChangeStatus(Status::None, Status::Init))
        {
            loop.reset(new EventLoop(event_share->EventLoop(event_index), event_index));
            {
                if (ChangeStatus(Status::Init, Status::Exec))
                {
                    if (loop->InitialEvent())
                    {
                        while (!loop_exit.load())
                        {
                            if (uv_loop_alive(loop->EventBasic()))
                            {
                                if (EventCoroutine::JoinInTask(event_coroutine_task.get()).SubmitTaskEx())
                                {
                                    continue;
                                }
                            }
                            
                            break;
                        }

                        if (0 || UV_EBUSY == uv_loop_close(loop->EventBasic()))
                        {
                            printf("Delete EventLoop Ok\n");
                        }

                        if (ChangeStatus(Status::Exec, Status::Stop))
                        {
                            assert(!loop->ExistsStoped());
                        }
                    }
                }
            }
        }
    }

    void EventLoopThreadEx::CoroutineDispatch()
    {
        if (nullptr != loop)
        {
            if (uv_loop_alive(loop->EventBasic()))
            {
                if (loop->ExecDispatch(UV_RUN_ONCE))
                {
                    printf("ExecDispatchEx Error\n");
                }
            }
        }
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