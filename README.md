# `libuv-evpp` 
1. 它拥有/轻量级/高性能/高内聚/低耦合/非阻塞纯异步的网络基础框架 采用 Reactor 模式封装

# 主要特性

1. 具有 epoll | kqueue | iocp | completionport | 功能齐全的事件循环
1. 非阻塞纯异步的 tcp 服务端以及客户端
1. 非阻塞纯异步的 dns 解析服务
1. 高可用的线程池以及定时器（时钟）
1. 它有非阻塞异步接口都是 C++11 的 functional/bind 形式的回调仿函数（并非 C 语言风格的函数指针）
1. CPU 多核友好和绝对的线程安全
1. TCP4 协议 / TCP6 协议
1. index 复用机制不会浪费系统资源
1. 高可用移植性强超低的耦合度 非常安全且平静的停止服务端（while 0 error）
1. 内部Dispatch预留了DoEvent接口方便Windows主线程遭受阻塞
1. 完全能够承受每天千亿级别的吞吐量，欢迎各路大神一起PR完善它

# 测试说明

1. Windows10 1S/20000 并发 > 测试软件：ApacheJMeter（由于Windows系统原因 本地测试只能达到2W个并发，因为端口受限）

# 编译说明

1. 请前往 [libuv](https://github.com/libuv/libuv) 必须
1. 请前往 [glog](https://github.com/google/glog) 非必须
1. 请前往 [concurrentqueue](https://github.com/cameron314/concurrentqueue) 非必须
1. 请前往 [boost](https://www.boost.org/) 非必须
1. 配置好依赖库即可编译，最低要求`C++2a`进行编译否则会有未知的错误（后续会降低C++版本）。

# libuv libevent 开发者交流群
1. QQ群: 730777001