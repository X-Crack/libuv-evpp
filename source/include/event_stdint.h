#ifndef __EVENT_STDINT_H__
#define __EVENT_STDINT_H__

#include <stdint.h>
#include <iostream>
#include <string>
#include <limits>

typedef char                        String;
typedef wchar_t                     LString;

typedef signed char                 i8;
typedef short                       i16;
typedef int                         i32;
typedef long long                   i64;
typedef unsigned char               u8;
typedef unsigned short              u16;
typedef unsigned int                u32;
typedef unsigned long long          u64;
#ifdef _WIN64
typedef __int64                     i96;
typedef unsigned __int64            u96;
#else
typedef int                         i96;
typedef unsigned int                u96;
#endif

#endif // __EVENT_STDINT_H__