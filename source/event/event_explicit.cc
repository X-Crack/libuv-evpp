#include <event_explicit.h>

namespace Evpp
{
    EventExplicit::EventExplicit()
    {

    }

    EventExplicit::EventExplicit(int argc, char* argv[])
    {
#ifndef GOOGLE_LOGGING
        FLAGS_log_prefix = true;
        FLAGS_colorlogtostderr = true;
        FLAGS_logbufsecs = 3;
        FLAGS_max_log_size = 10;
        FLAGS_v = 3;
        FLAGS_logtostderr = false;

        google::InitGoogleLogging(argv[0]);
        google::SetStderrLogging(google::GLOG_INFO);
        google::SetLogDestination(google::GLOG_INFO, argv[0]);
        google::InstallFailureSignalHandler();
#endif
    }

    EventExplicit::~EventExplicit()
    {
        system("pause");
    }
}