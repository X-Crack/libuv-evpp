#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__
#include <config.h>
#include <unordered_map>
#include <memory>
namespace Evpp
{
    class EventLoop;
    class EventWorkQueue
    {
    public:
        explicit EventWorkQueue(EventLoop* loop = nullptr);
        virtual ~EventWorkQueue();
    public:
        bool AssignWorkQueue(EventLoop* loop);
        bool AssignWorkQueue();
    public:
        void SetWorkCallback(const Handler& function);
        void SetAfterWorkCallback(const Handler& function);
    private:
        void OnWorkNotify(event_work* handler);
        void OnAfterWorkNotify(event_work* handler, int status);
    private:
        static void DefaultWorkNotify(event_work* handler);
        static void DefaultAfterWorkNotify(event_work* handler, int status);
    private:
        EventLoop*                                                              event_base;
        std::unordered_map<u96, std::unique_ptr<event_work>>                    event_work_pool;
        std::unique_ptr<event_work>                                             event_work_;
        Handler                                                                 work_callback;
        Handler                                                                 afterwork_callback;
    };
}
#endif // __EVENT_QUEUE_H__