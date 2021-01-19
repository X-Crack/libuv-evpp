#ifndef __EVENT_UNTILITY_H__
#define __EVENT_UNTILITY_H__
#include <platform.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <any>
#include <atomic>
namespace Evpp
{
	class EventLoop;
	class EventTimer;
	class TcpSession;
	class TcpBuffer;
	typedef std::function<void()>                                                                                                                           Handler;
	typedef std::function<bool()>                                                                                                                           Functor;
    typedef std::function<void(EventLoop*, const std::shared_ptr<TcpSession>&, const std::shared_ptr<EventTimer>&, const u96)>                              EventTimerHandler;
    typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const u96)>                                                                  InterfaceAccepts;
    typedef std::function<void(EventLoop*, const u96)>                                                                                                      InterfaceDiscons;
    typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const u96)>                                                                  InterfaceConnect;
	typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const std::shared_ptr<TcpBuffer>&, const u96)>                               InterfaceMessage;
	// InterfaceMessage InterfaceAccepts InterfaceDiscons InterfaceConnect
	enum EventThreadStatus
	{
		None			= 1 << 0,
		Init			= 1 << 1,
		Exec			= 1 << 2,
		Stop			= 1 << 3,
	};
}

#endif // __EVENT_UNTILITY_H__