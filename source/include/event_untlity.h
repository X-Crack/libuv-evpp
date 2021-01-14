#ifndef __EVENT_UNTILITY_H__
#define __EVENT_UNTILITY_H__
#include <platform.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <any>
#include <atomic>
namespace evpp
{
	typedef std::function<void()>																															Handler;
	typedef std::function<bool()>																															Functor;
	
	enum EventThreadStatus
	{
		None			= 1 << 0,
		Init			= 1 << 1,
		Exec			= 1 << 2,
		Stop			= 1 << 3,
	};
}

#endif // __EVENT_UNTILITY_H__