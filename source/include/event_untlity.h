#ifndef __EVENT_UNTILITY_H__
#define __EVENT_UNTILITY_H__
#include <event_config.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <any>
#include <atomic>
namespace Evpp
{
    class EventLoop;
    class EventTimer;
    class EventBuffer;
    class TcpSession;
    typedef std::function<void()>                                                                                                                           Handler;
    typedef std::function<bool()>                                                                                                                           Functor;
    typedef std::function<void(EventLoop*)>                                                                                                                 EventLoopHandler;
    typedef std::function<void(EventLoop*, const String*)>                                                                                                  EventResolveHandler;
    typedef std::function<void(EventLoop*, const std::vector<std::string>&)>                                                                                EventResolveExHandler;
    typedef std::function<void(EventLoop*)>                                                                                                                 CreaterWorkHandler;
    typedef std::function<void(EventLoop*, i32)>                                                                                                            DestroyWorkHandler;
    typedef std::function<void(EventLoop*, const std::shared_ptr<EventTimer>&, const u96)>                                                                  EventTimerHandle;
    typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const u96)>                                                                  InterfaceAccepts;
    typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const u96)>                                                                  InterfaceConnect;
    typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const u96)>                                                                  InterfaceRestore;
    typedef std::function<bool(EventLoop*, const u96, const i32, const String*, const String*)>                                                             InterfaceFailure;
    typedef std::function<bool(EventLoop*, const u96)>                                                                                                      InterfaceDiscons;
    typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const std::shared_ptr<EventBuffer>&, const u96)>                             InterfaceMessage;
    typedef std::function<bool(EventLoop*, const std::shared_ptr<TcpSession>&, const u96, const i32)>                                                       InterfaceSendMsg;

    namespace Import
    {
        // 接受连接
        bool DefaultAccepts(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index);
        // 断开连接
        bool DefaultDiscons(EventLoop* loop, const u96 index);
        // 接收消息
        bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<EventBuffer>& buffer, const u96 index);
        // 发送消息
        bool DefaultSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status);

        // 客户端 -> 连接成功
        bool DefaultConnect(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index);
        // 客户端 -> 断线重连
        bool DefaultRestore(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index);
        // 客户端 -> 连接失败
        bool DefaultFailure(EventLoop* loop, const u96 index, const i32 status, const String* name, const String* msgs);

        // 客户端拥有 DefaultConnect DefaultDiscons DefaultMessage DefaultSendMsg DefaultRestore DefaultFailure
        // 服务端拥有 DefaultAccepts DefaultDiscons DefaultMessage DefaultSendMsg
    };

    struct SocketInfo final
    {
        union
        {
            u16                 family;
            sockaddr            addr;
            sockaddr_in         addr4;
            sockaddr_in6        addr6;
        };
        std::string             host;
        u16                     port;
    };

    struct SocketInfoEx
    {
        SocketInfo              sockname;
        SocketInfo              peername;
    };

}

#endif // __EVENT_UNTILITY_H__
