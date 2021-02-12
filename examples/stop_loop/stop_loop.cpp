#include "stop_loop.h"
#include <event_loop.h>
#include <event_signal.h>
namespace Evpp
{
    StopLoop::StopLoop() : exit_tag(0)
    {
        event_base.reset(new EventLoop());
        event_signal_.reset(new EventSignal(event_base.get(), std::bind(&StopLoop::EchoSignalCallback, this)));
    }

    StopLoop::~StopLoop()
    {
        
    }

    void StopLoop::ExitLoop(EventLoop* loop)
    {
        while (0 == exit_tag);

        if (loop->ExistsRuning())
        {
            loop->StopDispatch();
        }
    }

    void StopLoop::SetExitTag()
    {
        exit_tag.store(1);
    }

    void StopLoop::RunExamples1()
    {
        socket_tcp server;
        sockaddr_in addr;

        uv_ip4_addr("0.0.0.0", 6666, &addr);
        uv_tcp_init(event_base->EventBasic(), &server);
        uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
        uv_listen((socket_stream*)&server, 128, 0);

        std::thread exit_thread(std::bind(&StopLoop::ExitLoop, this, event_base.get()));
        std::thread set_tag_thread(std::bind(&StopLoop::SetExitTag, this));

        if (event_base->InitialEvent())
        {
            event_base->ExecDispatch();
        }
        

        exit_thread.join();
        set_tag_thread.join();

        std::cout << "EventLoop Exit" << std::endl;
        std::system("pause");
    }


    void StopLoop::RunExamples2()
    {
        socket_tcp server;
        sockaddr_in addr;

        uv_ip4_addr("0.0.0.0", 6666, &addr);
        uv_tcp_init(event_base->EventBasic(), &server);
        uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
        uv_listen((socket_stream*)&server, 128, 0);

        if (event_base->InitialEvent())
        {
            event_base->ExecDispatchEx(std::bind(&StopLoop::EchoLoopCallback, this, std::placeholders::_1), UV_RUN_NOWAIT);
        }

        std::cout << "EventLoop Exit" << std::endl;
        std::system("pause");
    }

    void StopLoop::EchoLoopCallback(EventLoop* loop)
    {
        if (++exit_tag > 9)
        {
            loop->StopDispatch();
        }

        std::cout << "Hello EventLoop: " << exit_tag << std::endl;
    }

    void StopLoop::RunExamples3()
    {
        socket_tcp server;
        sockaddr_in addr;

        uv_ip4_addr("0.0.0.0", 6666, &addr);
        uv_tcp_init(event_base->EventBasic(), &server);
        uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
        uv_listen((socket_stream*)&server, 128, 0);

        event_signal_->InitialSignal();
        // SIGHUP = WM_CLOSE
        // SIGINT = Ctrl+C 
        // SIGBREAK = Ctrl+Break
                /*
        http://msdn.microsoft.com/en-us/library/windows/desktop/ms682541(v=vs.85).aspx
        0x74C312B5 (KernelBase.dll)处(位于 ConsoleApplication1.exe 中)引发的异常: 0x40010005: Control-C
        在弹出的 已引发异常窗口 的异常设置下 引发此异常类型时中断 下 从以下位置引发时除外 下 KernelBase.dll 选上勾就行了，或者在 调试>窗口>异常设置 (Ctrl + Alt + E) 下的 Win32 Exceptions 项下的 0x40010005 选定右键 编辑条件 模块名 不等于 KernelBase.dll 就 Ok 了
        The CTRL+C and CTRL+BREAK key combinations receive special handling by console processes. By default, when a console window has the keyboard focus, CTRL+C or CTRL+BREAK is treated as a signal (SIGINT or SIGBREAK) and not as keyboard input. By default, these signals are passed to all console processes that are attached to the console. (Detached processes are not affected. See Creation of a Console.) The system creates a new thread in each client process to handle the event. The thread raises an exception if the process is being debugged. The debugger can handle the exception or continue with the exception unhandled.
        */
        event_signal_->CreaterSignal(SIGINT);
        // TODO: Func Set Callback
        event_signal_->SetSignalCallback(std::bind(&StopLoop::EchoSignalCallback, this));

        if (event_base->InitialEvent())
        {
            event_base->ExecDispatchEx(std::bind(&StopLoop::EchoLoopCallback3, this, std::placeholders::_1), UV_RUN_NOWAIT);
        }

        std::cout << "EventLoop Exit" << std::endl;
        std::system("pause");
    }

    void StopLoop::EchoLoopCallback3(EventLoop* loop)
    {
        if (exit_tag)
        {
            // TODO: DestroySignal;
            loop->StopDispatch();
        }

        std::cout << "Hello EventLoop: " << exit_tag << std::endl;
    }

    void StopLoop::EchoSignalCallback()
    {
        exit_tag.store(1);
    }

    void StopLoop::RunExamples4()
    {
        socket_tcp server;
        sockaddr_in addr;

        uv_ip4_addr("0.0.0.0", 6666, &addr);
        uv_tcp_init(event_base->EventBasic(), &server);
        uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
        uv_listen((socket_stream*)&server, 128, 0);

        if (event_base->InitialEvent())
        {
            std::thread exit_thread(std::bind(&StopLoop::EchoLoopCallback4, this, event_base.get()));
            event_base->ExecDispatch();
            exit_thread.join();
        }

        std::cout << "EventLoop Exit" << std::endl;
        std::system("pause");
    }

    void StopLoop::EchoLoopCallback4(EventLoop* loop)
    {
        loop->StopDispatch();
        std::cout << "Hello EventLoop: " << exit_tag << std::endl;
    }
}