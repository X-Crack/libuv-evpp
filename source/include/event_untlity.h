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
        NOFORCEINLINE bool DefaultAccepts(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index);
        // 断开连接
        NOFORCEINLINE bool DefaultDiscons(EventLoop* loop, const u96 index);
        // 接收消息
        NOFORCEINLINE bool DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<EventBuffer>& buffer, const u96 index);
        // 发送消息
        NOFORCEINLINE bool DefaultSendMsg(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index, const i32 status);

        // 客户端 -> 连接成功
        NOFORCEINLINE bool DefaultConnect(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index);
        // 客户端 -> 断线重连
        NOFORCEINLINE bool DefaultRestore(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const u96 index);
        // 客户端 -> 连接失败
        NOFORCEINLINE bool DefaultFailure(EventLoop* loop, const u96 index, const i32 status, const String* name, const String* msgs);

        // 客户端拥有 DefaultConnect DefaultDiscons DefaultMessage DefaultSendMsg DefaultRestore DefaultFailure
        // 服务端拥有 DefaultAccepts DefaultDiscons DefaultMessage DefaultSendMsg
    };

    struct SocketInfo final
    {
        union
        {
            u16                 family;
            sockaddr_storage    addr_storage;
            sockaddr            addr;
            sockaddr_in         addr4;
            sockaddr_in6        addr6;
        };
        std::string             host;
        u16                     port;
        String                  host_address[256];
    };

    struct SocketInfoEx
    {
        SocketInfo              sockname;
        SocketInfo              peername;
    };

    NOFORCEINLINE bool EventLoopAlive(event_loop* loop);
    void DoDispatchEvent();

    struct HashBit
    {
#ifdef H_OS_X86
        static constexpr const u96 Hash(const char* hash, const u96 size, u96 hash_mask = 0x90cbfa3b, u32 index = 0);
#elif  H_OS_X64
        static constexpr const u96 Hash(const char* hash, const u96 size, u96 hash_mask = 0xf4fea0fe1a79ec80, u32 index = 0);
#endif
    };

    bool SocketFormatErrorString(u32 code, String* format_string);
    
    template <class _Ty>
    bool SocketStatus(_Ty* handler)
    {
        if (nullptr != handler)
        {
            if (uv_is_active(reinterpret_cast<event_handle*>(handler)))
            {
                return 0 == uv_is_closing(reinterpret_cast<event_handle*>(handler));
            }
            return true;
        }
        return false;
    }

    template <class _Ty>
    bool SocketShutdown(_Ty* handler, uv_shutdown_cb callback)
    {
        if (nullptr != handler)
        {
            // 不指定 .data = handler 即为默认第一个参数 如指定 .data = handler 既需要 C++2A
            return 0 == uv_shutdown(new socket_shutdown({ handler }), reinterpret_cast<socket_stream*>(handler), callback);
        }
        return false;
    }

    template <class _Ty>
    bool SocketClose(_Ty* handler, uv_close_cb callback)
    {
        if (nullptr != handler)
        {
            if (0 == uv_is_closing(reinterpret_cast<event_handle*>(handler)))
            {
                uv_close(reinterpret_cast<event_handle*>(handler), callback);
                {
                    return true;
                }
            }
        }
        return false;
    }
}
#endif // __EVENT_UNTILITY_H__