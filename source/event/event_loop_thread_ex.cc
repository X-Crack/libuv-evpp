#include <event_status.h>
#include <event_loop.h>
#include <event_loop_thread_ex.h>
#include <event_share.h>
#include <event_coroutine.h>
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
        event_loop(std::make_shared<EventLoop>(event_share->EventLoop(index), index)),
        loop_exit(0)
    {

    }

    EventLoopThreadEx::~EventLoopThreadEx()
    {
        if (Join())
        {

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
                        // �����߳���Ҫ����������Ϊ��ʼ�����ݹ��࣬����ᵼ��RunInLoop�첽��ȫ��ʼ��ʧ�ܡ�
                        if (cv_signal.wait_for(lock, std::chrono::milliseconds(64), std::bind(&EventLoopThreadEx::AvailableEvent, this)))
                        {
                            return true;
                        }
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
        if (nullptr != event_loop)
        {
            if (uv_loop_alive(event_loop->EventBasic()))
            {
                if (0 == loop_exit)
                {
                    loop_exit.store(1);
                }
                return event_loop->StopDispatch();
            }
        }
        return false;
    }

    EventLoop* EventLoopThreadEx::GetEventLoop()
    {
        if (nullptr != event_loop)
        {
            return event_loop.get();
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
            if(nullptr != event_loop)
            {
                if (ChangeStatus(Status::Init, Status::Exec))
                {
                    if (event_loop->InitialEvent())
                    {
                        while (!loop_exit.load())
                        {
                            if (uv_loop_alive(event_loop->EventBasic()))
                            {
                                try
                                {
                                    EventCoroutine::JoinInTask(std::bind(&EventLoopThreadEx::CoroutineDispatch, this));
                                }
                                catch (...)
                                {
                                	break;
                                }
                            }
                        }

                        if (0 || UV_EBUSY == uv_loop_close(event_loop->EventBasic()))
                        {
                            printf("Delete EventLoop Ok %d\n", event_index);
                        }

                        if (ChangeStatus(Status::Exec, Status::Stop))
                        {
                            assert(!event_loop->ExistsStoped());
                        }
                    }
                }
            }
        }
    }

    bool EventLoopThreadEx::CoroutineDispatch()
    {
        if (nullptr != event_loop)
        {
            if (uv_loop_alive(event_loop->EventBasic()))
            {
                return event_loop->ExecDispatch(UV_RUN_ONCE);
            }
        }
        return false;
    }

    bool EventLoopThreadEx::AvailableEvent()
    {
        if (nullptr == event_loop)
        {
            return false;
        }

        if (ExistsRuning())
        {
            return 0 == event_loop->EventBasic()->stop_flag;
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