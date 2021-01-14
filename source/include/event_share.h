#ifndef __EVENT_SHARE_H__
#define __EVENT_SHARE_H__
#include <config.h>
#include <unordered_map>
#include <mutex>
namespace evpp
{
    class EventShare
    {
    public:
        explicit EventShare();
        explicit EventShare(const u96 size = 1);
        virtual ~EventShare();
    public:
        bool CreaterLoops();
        bool DestroyLoops();
        u32  GetLoopsSize() { return event_size; };
    public:
        event_loop* EventLoop(const u96 index = 0);
        event_loop* DefaultEventLoop();
    private:
        event_loop* CreaterDefaultEventLoop();
    private:
        event_loop*                                                                 event_default;
        u96                                                                         event_size;
        std::unordered_map<u96, event_loop*>                                        event_loops;
        std::mutex										                            event_mutex;
    };
}
#endif // __EVENT_SHARE_H__