#ifndef __EVENT_RESOLVE_H__
#define __EVENT_RESOLVE_H__
#include <config.h>
#include <string>
namespace Evpp
{
    class EventLoop;
    class EventResolve
    {
    public:
        explicit EventResolve(EventLoop* loop);
        virtual ~EventResolve();
    public:
        bool GetAddressInfo(const std::string& hostname, const std::string& service);
    private:
        void DefaultGetaddrInfo(socket_getaddrinfo* request, struct addrinfo* hints);
        static void DefaultGetaddrInfo(socket_getaddrinfo* request, int status, struct addrinfo* hints);
    private:
        EventLoop*                                                                  event_loop;
        socket_getaddrinfo*                                                         socket_resolve;
        addrinfo                                                                    socket_hints;
    };
}
#endif // __EVENT_RESOLVE_H__