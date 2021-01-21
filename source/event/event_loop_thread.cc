#include <event_loop_thread.h>
#include <event_share.h>
namespace Evpp
{
    EventLoopThread::EventLoopThread(const u96 index) : event_loop(nullptr), event_index(index)
    {

    }

    EventLoopThread::EventLoopThread(EventLoop* loop, const std::shared_ptr<EventShare>& share, const u96 index) : event_loop(loop), event_share(share), event_index(index)
    {
        
    }

    EventLoopThread::~EventLoopThread()
    {
        loop_thread.reset();
    }

    bool EventLoopThread::CreaterEventLoopThread(bool wait)
    {
        if (nullptr == loop_thread && event_loop)
        {
            if (event_loop->SelftyThread())
            {
                loop_thread.reset(new std::thread(std::bind(&EventLoopThread::Run, this)));
                {
                    if (wait)
                    {
                        std::unique_lock<std::mutex> lock(cv_mutex);
                        {
                            if (cv_signal.wait_for(lock, std::chrono::milliseconds(8), std::bind(&EventLoopThread::AvailableEvent, this)))
                            {
                                return this->Join();
                            }
                        }
                    }
                }
                return this->Join();
            }
            return event_loop->RunInLoop(std::bind(&EventLoopThread::CreaterEventLoopThread, this, wait));
        }
        return false;
    }

    bool EventLoopThread::Join()
    {
        if (loop_thread && loop_thread->joinable())
        {
            loop_thread->detach();
            return true;
        }
        return false;
    }

    EventLoop* EventLoopThread::GetEventLoop()
    {
        if (nullptr != loop)
        {
            return loop.get();
        }

        if (nullptr != event_loop)
        {
            return event_loop;
        }

        return nullptr;
    }

    bool EventLoopThread::AvailableEvent()
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

    void EventLoopThread::Run()
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
}