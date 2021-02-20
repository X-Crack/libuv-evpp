#ifndef __EVENT_EXPORT_H__
#define __EVENT_EXPORT_H__
#include <event_arch.h>

#ifndef EVPP_STATIC
#   define EVPP_STATIC
#endif

#ifdef __cplusplus
#   if __cplusplus > 201703L
#       define EVPP_CPLUSPLUS_VERSION(x) x##" "##"C++ 2A"
#   elif __cplusplus == 201703L
#       define EVPP_CPLUSPLUS_VERSION(x) x##" "##"C++ 17"
#   elif __cplusplus == 201402L
#       define EVPP_CPLUSPLUS_VERSION(x) x##" "##"C++ 14"
#   elif __cplusplus == 201103L
#       define EVPP_CPLUSPLUS_VERSION(x) x##" "##"C++ 11"
#   elif __cplusplus == 199711L
#       define EVPP_CPLUSPLUS_VERSION(x) x##" "##"C++ 98"
#else
#       define EVPP_CPLUSPLUS_VERSION(x) x##" "##"C++ Unknown"
#   endif
#endif

#ifdef H_OS_WINDOWS
#   if !defined(EVPP_DEV_OUTPUT_LOG)
#       define EVPP_DEV_OUTPUT_LOG            EVPP_CPLUSPLUS_VERSION
#   endif
#   if !defined(_DLL) && !defined(_DEBUG)
#       ifndef EVPP_STATIC
#           ifndef EVPP_EXPORT_STATIC
#               define EVPP_EXPORT_STATIC
#           endif
#       endif
#       pragma message(EVPP_DEV_OUTPUT_LOG("Compilation mode MT"))
#   elif !defined(_DLL) && defined(_DEBUG)
#       ifndef EVPP_STATIC
#           ifndef EVPP_EXPORT_STATIC
#               define EVPP_EXPORT_STATIC
#           endif
#       endif
#       pragma message(EVPP_DEV_OUTPUT_LOG("Compilation mode MTd"))
#   elif defined(_DLL) && !defined(_DEBUG)
#       ifndef EVPP_STATIC
#           ifdef EVPP_EXPORT_STATIC
#               undef EVPP_EXPORT_STATIC
#           endif
#       endif
#       pragma message(EVPP_DEV_OUTPUT_LOG("Compilation mode MD"))
#   elif defined(_DLL) && defined(_DEBUG)
#       ifndef EVPP_STATIC
#           ifdef EVPP_EXPORT_STATIC
#               undef EVPP_EXPORT_STATIC
#           endif
#       endif
#       pragma message(EVPP_DEV_OUTPUT_LOG("Compilation mode MDd"))
#   endif
#endif

#if defined(H_OS_WINDOWS)
#       define EVPP_DECL_EXPORT __declspec(dllexport)
#       define EVPP_DECL_IMPORT __declspec(dllimport)
#else
#       define EVPP_DECL_EXPORT
#       define EVPP_DECL_IMPORT
#endif

#ifndef EVPP_STATIC
#   if defined(EVPP_EXPORT_STATIC)
#       define EVPP_EXPORT EVPP_DECL_EXPORT
#   else
#       define EVPP_EXPORT EVPP_DECL_IMPORT
#   endif
#else
#       define EVPP_EXPORT
#endif

#endif  // __EVENT_EXPORT_H__