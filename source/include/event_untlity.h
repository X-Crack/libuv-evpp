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
	typedef std::function<void(EventLoop*)>                                                                                                                 EventLoopHandler;
	typedef std::function<void(EventLoop*, const String*)>																									EventResolveHandler;
	typedef std::function<void(EventLoop*, const std::vector<std::string>&)>                                                                                EventResolveExHandler;
	typedef std::function<void(EventLoop*)>                                                                                                                 CreaterWorkHandler;
	typedef std::function<void(EventLoop*, i32)>                                                                                                            DestroyWorkHandler;
    typedef std::function<void(EventLoop*, const std::shared_ptr<EventTimer>&, const u96)>																	EventTimerHandle;
    typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const u96)>                                                                  InterfaceAccepts;
    typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const u96)>                                                                  InterfaceConnect;
	typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const u96)>                                                                  InterfaceRestore;
    typedef std::function<bool(EventLoop*, const u96, const i32, const String*, const String*)>																InterfaceFailure;
	typedef std::function<bool(EventLoop*, const u96)>                                                                                                      InterfaceDiscons;
	typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const std::shared_ptr<TcpBuffer>&, const u96)>                               InterfaceMessage;
	typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const u96, const i32)>                                                       InterfaceSendMsg;
	
	namespace Import
    {
		bool DefaultAccepts(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index);

		bool DefaultDiscons(EventLoop* loop, const u96 index);

        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index);

		bool DefaultSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status);

		bool DefaultConnect(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index);

		bool DefaultRestore(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index);

		bool DefaultFailure(EventLoop* loop, const u96 index, const i32 status, const String* name, const String* msgs);
	}
}

#endif // __EVENT_UNTILITY_H__