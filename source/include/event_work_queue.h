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
        void SetCreaterCallback(const CreaterWorkHandler& function);
        void SetDestroyCallback(const DestroyWorkHandler& function);
    private:
        void OnCreaterNotify();
        void OnDestroyNotify(int status);
    private:
        static void OnCreaterNotify(event_work* handler);
        static void OnDestroyNotify(event_work* handler, int status);
    private:
        EventLoop*                                                              event_base;
        CreaterWorkHandler                                                      creater_callback;
        DestroyWorkHandler                                                      destroy_callback;
    };
}
#endif // __EVENT_QUEUE_H__