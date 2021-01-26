#include <event_resolve.h>
#include <event_loop.h>
namespace Evpp
{
    namespace
    {
        #ifdef H_OS_WINDOWS
            #define IPPROTO_IPV4_PROTOCOL IPPROTO_IPV4
            #define IPPROTO_IPV6_PROTOCOL IPPROTO_IPV6
        #else
            #define IPPROTO_IPV4_PROTOCOL IPPROTO_IPIP
            #define IPPROTO_IPV6_PROTOCOL IPPROTO_IPV6
        #endif
    }

    EventResolve::EventResolve(EventLoop* loop) : 
        event_base(loop),
        socket_resolve(new socket_getaddrinfo()), 
        socket_hints()
    {
        if (nullptr == socket_resolve->data)
        {
            socket_resolve->data = this;
        }
    }

    EventResolve::~EventResolve()
    {
        if (nullptr != socket_resolve)
        {
            delete socket_resolve;
            socket_resolve = nullptr;
        }
    }

    bool EventResolve::GetAddressInfo(const std::string& hostname, const std::string& service)
    {
        return 0 == uv_getaddrinfo(event_base->EventBasic(), socket_resolve, &EventResolve::DefaultGetaddrInfo, hostname.c_str(), service.c_str(), &socket_hints);
    }

    void EventResolve::DefaultGetaddrInfo(struct addrinfo* hints)
    {
        String ipaddress[64] = {};
        {
            if (nullptr != hints)
            {
                if (hints->ai_protocol == IPPROTO_IPV4_PROTOCOL || hints->ai_protocol == IPPROTO_TCP || hints->ai_protocol == IPPROTO_IP)
                {
                    if (0 == uv_ip4_name(reinterpret_cast<struct sockaddr_in*>(hints->ai_addr), ipaddress, std::size(ipaddress)))
                    {
                        return;
                    }
                }

                if (hints->ai_protocol == IPPROTO_IPV6_PROTOCOL)
                {
                    if (0 == uv_ip6_name(reinterpret_cast<struct sockaddr_in6*>(hints->ai_addr), ipaddress, std::size(ipaddress)))
                    {
                        return;
                    }
                }
            }
        }
    }

    void EventResolve::DefaultGetaddrInfo(socket_getaddrinfo* request, int status, struct addrinfo* hints)
    {
        if (nullptr != request && 0 == status)
        {
            EventResolve* watcher = static_cast<EventResolve*>(request->data);
            {
                if (nullptr != watcher)
                {
                    watcher->DefaultGetaddrInfo(hints);
                }
            }
        }
    }
}