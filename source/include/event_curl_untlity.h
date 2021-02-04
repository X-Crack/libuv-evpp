#ifndef __EVENT_CURL_UNTLITY_H__
#define __EVENT_CURL_UNTLITY_H__
#include <event_stdint.h>
#include <functional>
namespace Evpp
{
    typedef std::function<bool(const u96, const double, const double, const double, const double)>                                         CurlProgressHandler;
    typedef std::function<u96(const u96, void*, const u96, const u96)>                                                                     CurlMessageHandler;
    typedef std::function<void(const u96, const i32)>                                                                                      CurlMessageTaskHandler;
}

#endif // __EVENT_CURL_UNTLITY_H__
