#include "stop_loop.h"
#include <event_loop.h>
#include <event_timer.h>
namespace Evpp
{
    StopLoop::StopLoop() : exit_tag(0)
    {
        event_base.reset(new EventLoop());
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
            event_base->ExecDispatch(std::bind(&StopLoop::EchoLoopCallback, this), UV_RUN_NOWAIT);
        }

        std::cout << "EventLoop Exit" << std::endl;
    }

    void StopLoop::EchoLoopCallback()
    {
        if (++exit_tag > 9)
        {
            event_base->StopDispatch();
        }

        std::cout << "Hello EventLoop: " << exit_tag << std::endl;
    }
}