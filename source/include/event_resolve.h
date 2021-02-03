#ifndef __EVENT_RESOLVE_H__
#define __EVENT_RESOLVE_H__
#include <event_config.h>
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
        bool GetAddressInfo(const std::string& hostname, const std::string& service = std::string());
        void SetGetaddrInfoCallback(const EventResolveHandler& callback);
        void SetGetaddrInfoExCallback(const EventResolveExHandler& callback);
    private:
        void DefaultGetaddrInfo(struct addrinfo* hints);
        static void DefaultGetaddrInfo(socket_getaddrinfo* request, int status, struct addrinfo* hints);
    private:
        EventLoop* event_base;
        EventResolveHandler                                                         event_callback;
        EventResolveExHandler                                                       event_callback_ex;
        std::vector<std::string>                                                    socket_list;
        socket_getaddrinfo* socket_resolve;
        addrinfo                                                                    socket_hints;
    };
}
#endif // __EVENT_RESOLVE_H__
