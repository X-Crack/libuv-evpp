#ifndef __EVENT_SIGNAL_H__
#define __EVENT_SIGNAL_H__
#include <event_config.h>
namespace Evpp
{
    class EventLoop;
    class EVPP_EXPORT EventSignal
    {
    public:
        explicit EventSignal(EventLoop* loop, const Handler& function = Handler());
        virtual ~EventSignal();
    public:
        bool InitialSignal();
        /*
        http://msdn.microsoft.com/en-us/library/windows/desktop/ms682541(v=vs.85).aspx
        0x74C312B5 (KernelBase.dll)处(位于 ConsoleApplication1.exe 中)引发的异常: 0x40010005: Control-C
        在弹出的 已引发异常窗口 的异常设置下 引发此异常类型时中断 下 从以下位置引发时除外 下 KernelBase.dll 选上勾就行了，或者在 调试>窗口>异常设置 (Ctrl + Alt + E) 下的 Win32 Exceptions 项下的 0x40010005 选定右键 编辑条件 模块名 不等于 KernelBase.dll 就 Ok 了
        The CTRL+C and CTRL+BREAK key combinations receive special handling by console processes. By default, when a console window has the keyboard focus, CTRL+C or CTRL+BREAK is treated as a signal (SIGINT or SIGBREAK) and not as keyboard input. By default, these signals are passed to all console processes that are attached to the console. (Detached processes are not affected. See Creation of a Console.) The system creates a new thread in each client process to handle the event. The thread raises an exception if the process is being debugged. The debugger can handle the exception or continue with the exception unhandled.
        */
        bool CreaterSignal(const i32 signum = SIGHUP);
        bool DestroySignal();
    public:
        void SetSignalCallback(const Handler& function);
    private:
        static void OnNotify(event_signal* handler, int signum);
        void OnNotify();
    private:
        EventLoop*                                                              event_base;
        event_signal*                                                           event_signaler;
        Handler                                                                 event_callback;
    };
}
#endif // __EVENT_SIGNAL_H__
