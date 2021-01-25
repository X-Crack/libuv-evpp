#include <event_status.h>
#include <event_loop.h>
#include <event_share.h>
#include <event_loop_thread.h>
#include <event_work_queue.h>
#include <future>
#include <thread>
namespace Evpp
{
    EventLoopThread::EventLoopThread(const u96 index) : event_base(nullptr), event_index(index)
    {

    }

    EventLoopThread::EventLoopThread(EventLoop* loop, const std::shared_ptr<EventShare>& share, const u96 index) : 
        event_base(loop), 
        event_share(share), 
        event_index(index),
        event_thread(std::make_unique<uv_thread_t>())
    {
        
    }

    EventLoopThread::~EventLoopThread()
    {
        DestroyThread();
    }

    bool EventLoopThread::CreaterThread(bool wait)
    {
        if (nullptr != event_base)
        {
            if (event_base->EventThread())
            {
                if (this->CreaterThread())
                {
                    if (wait)
                    {
                        return DestroyThread();
                    }
                    return true;
                }
            }
            return event_base->RunInLoop(std::bind((bool(EventLoopThread::*)(bool))&EventLoopThread::CreaterThread, this, wait));
        }
        return false;
    }

    EventLoop* EventLoopThread::GetEventLoop()
    {
        if (nullptr != loops_base)
        {
            return loops_base.get();
        }

        if (nullptr != event_base)
        {
            return event_base;
        }

        return nullptr;
    }

    bool EventLoopThread::CreaterThread()
    {
        return 0 == uv_thread_create(event_thread.get(), &EventLoopThread::ThreadRun, this);
    }

    bool EventLoopThread::DestroyThread()
    {
        return 0 == uv_thread_join(event_thread.get());
    }

    void EventLoopThread::ThreadRun()
    {
        if (ChangeStatus(NOTYET, INITIALIZING))
        {
            loops_base.reset(new EventLoop(event_share->EventLoop(event_index), event_index));
            {
                if (ChangeStatus(INITIALIZING, INITIALIZED))
                {
                    if (loops_base->InitialEvent())
                    {
                        if (false == loops_base->ExecDispatch())
                        {
                            assert(0);
                            return;
                        }

                        if (ChangeStatus(INITIALIZED, STOPPED))
                        {
                            assert(loops_base->ExistsStoped());
                        }
                    }
                }
            }
        }
    }

    void EventLoopThread::ThreadRun(void* handler)
    {
        if (nullptr != handler)
        {
            EventLoopThread* watcher = static_cast<EventLoopThread*>(handler);
            {
                if (nullptr != watcher)
                {
                    watcher->ThreadRun();
                }
            }
        }
    }
}