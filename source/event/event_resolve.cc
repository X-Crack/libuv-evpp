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
        if (socket_list.size())
        {
            socket_list.clear();
            socket_list.shrink_to_fit();
        }
        return 0 == uv_getaddrinfo(event_base->EventBasic(), socket_resolve, &EventResolve::DefaultGetaddrInfo, hostname.c_str(), service.c_str(), &socket_hints);
    }

    void EventResolve::SetGetaddrInfoCallback(const EventResolveHandler& callback)
    {
        if (nullptr == event_callback)
        {
            event_callback = callback;
        }
    }

    void EventResolve::SetGetaddrInfoExCallback(const EventResolveExHandler& callback)
    {
        if (nullptr == event_callback)
        {
            event_callback_ex = callback;
        }
    }

    void EventResolve::DefaultGetaddrInfo(struct addrinfo* hints)
    {
        String ipaddress[128];
        {
            if (nullptr != hints)
            {
                if (hints->ai_protocol == IPPROTO_IPV4_PROTOCOL || hints->ai_protocol == IPPROTO_TCP || hints->ai_protocol == IPPROTO_IP)
                {
                    for (struct addrinfo* ai_next = hints; ai_next; ai_next = ai_next->ai_next)
                    {
                        if (0 == uv_ip4_name(reinterpret_cast<struct sockaddr_in*>(ai_next->ai_addr), ipaddress, std::size(ipaddress)))
                        {
                            if (nullptr == event_callback)
                            {
                                socket_list.push_back(ipaddress);
                                continue;
                            }
                            
                            if (nullptr != event_callback)
                            {
                                event_callback(event_base, ipaddress);
                                continue;
                            }
                        }
                        break;
                    }
                }

                if (hints->ai_protocol == IPPROTO_IPV6_PROTOCOL)
                {
                    for (struct addrinfo* ai_next = hints; ai_next; ai_next = ai_next->ai_next)
                    {
                        if (0 == uv_ip6_name(reinterpret_cast<struct sockaddr_in6*>(ai_next->ai_addr), ipaddress, std::size(ipaddress)))
                        {
                            if (nullptr == event_callback)
                            {
                                socket_list.push_back(ipaddress);
                                continue;
                            }

                            if (nullptr != event_callback)
                            {
                                event_callback(event_base, ipaddress);
                                continue;
                            }
                        }
                        break;
                    }
                }

                if (nullptr != event_callback_ex)
                {
                    event_callback_ex(event_base, socket_list);
                }
            }
        }
    }

    void EventResolve::DefaultGetaddrInfo(socket_getaddrinfo* request, int status, struct addrinfo* hints)
    {
        String error_name[96];
        String error_msgs[96];
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

        printf("%s %s\n", uv_err_name_r(status, error_name, std::size(error_name)), uv_strerror_r(status, error_msgs, std::size(error_msgs)));
    }
}