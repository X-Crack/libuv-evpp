#ifndef __EVENT_EXPLICIT_H__
#define __EVENT_EXPLICIT_H__
#include <event_config.h>
namespace Evpp
{
    class EventExplicit
    {
    public:
        explicit EventExplicit();
        explicit EventExplicit(int argc, char* argv[]);
        virtual ~EventExplicit();
    public:
    private:
    };
}
#endif // __EVENT_EXPLICIT_H__