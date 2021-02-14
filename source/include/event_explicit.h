#ifndef __EVENT_EXPLICIT_H__
#define __EVENT_EXPLICIT_H__
#include <event_config.h>
namespace Evpp
{
#ifdef H_OS_WINDOWS
    class EventExplicit
    {
    public:
        explicit EventExplicit()
        {
#ifndef GOOGLE_LOGGING
            FLAGS_log_prefix = true;
            FLAGS_colorlogtostderr = true;
            FLAGS_logbufsecs = 3;
            FLAGS_max_log_size = 10;
            FLAGS_v = 3;
            FLAGS_logtostderr = false;

            google::InitGoogleLogging("./");
            google::SetStderrLogging(google::GLOG_INFO);
            google::SetLogDestination(google::GLOG_INFO, "./");
            google::InstallFailureSignalHandler();
#endif
        }

        virtual ~EventExplicit()
        {
            std::system("pause");
        }
    } /*__s_onexit_pause*/;
#endif
}
#endif // __EVENT_EXPLICIT_H__
