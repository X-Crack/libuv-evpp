#ifndef __STOP_LOOP_H__
#define __STOP_LOOP_H__
#include <event_config.h>
#include <memory>
#include <thread>
namespace Evpp
{
    class EventLoop;
    class EventSignal;
    class StopLoop
    {
    public:
        explicit StopLoop();
        virtual ~StopLoop();
    public:
        // Examples 1
        void RunExamples1();
        void ExitLoop(EventLoop* loop);
        void SetExitTag();
        // Examples 2
        void RunExamples2();
        void EchoLoopCallback(EventLoop* loop);
        // Examples 3
        void RunExamples3();
        void EchoLoopCallback3(EventLoop* loop);
        void EchoSignalCallback();
        // Examples 4
        void RunExamples4();
        void EchoLoopCallback4(EventLoop* loop);
    private:
        std::shared_ptr<EventLoop>              event_base;
        std::shared_ptr<EventSignal>            event_signal_;
        std::atomic<u32>                        exit_tag;
    };
}
#endif // __STOP_LOOP_H__