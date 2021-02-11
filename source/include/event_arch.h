#ifndef __EVENT_ARCH_H__
#define __EVENT_ARCH_H__
#include <ctype.h>
#include <cassert>
#ifdef __CYGWIN__
#   define H_OS_CYGWIN                                                                          1
#   elif defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)
#       define H_OS_WINDOWS                                                                     1
#       define H_OS_WINDOWS_MINGW                                                               1
#   elif defined(_MSC_VER) && (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(_WIN64_) || defined(WIN64))
#       define H_OS_WINDOWS                                                                     1
#       define H_OS_WINDOWS_MSVC                                                                1
#   ifdef _WIN32_WCE
#       define H_OS_WINDOWS_MOBILE                                                              1
#   elif defined(WINAPI_FAMILY)
#       include <winapifamily.h>
#   if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#       define H_OS_WINDOWS_DESKTOP                                                             1
#   elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE_APP)
#       define H_OS_WINDOWS_PHONE                                                               1
#   elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
#       defineH_OS_WINDOWS_RT                                                                   1
#   elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_TV_TITLE)
#       define H_OS_WINDOWS_PHONE                                                               1
#       define H_OS_WINDOWS_TV_TITLE                                                            1
#   else
#       define H_OS_WINDOWS_DESKTOP                                                             1
#   endif
#   else
#       define H_OS_WINDOWS_DESKTOP                                                             1
#   endif  // _WIN32_WCE
#   elif defined __OS2__
#       define H_OS_OS2                                                                         1
#   elif defined __APPLE__
#       define H_OS_MAC                                                                         1
#       include <TargetConditionals.h>
#   if TARGET_OS_IPHONE
#       define H_OS_IOS                                                                         1
#   endif
#   elif defined __DragonFly__
#       define H_OS_DRAGONFLY                                                                   1
#   elif defined __FreeBSD__
#       define H_OS_FREEBSD                                                                     1
#   elif defined __Fuchsia__
#       define H_OS_FUCHSIA                                                                     1
#   elif defined(__GLIBC__) && defined(__FreeBSD_kernel__)
#       define H_OS_GNU_KFREEBSD                                                                1
#   elif defined __linux__
#       define H_OS_LINUX                                                                       1
#   if defined __ANDROID__
#       define H_OS_LINUX_ANDROID                                                               1
#   endif
#   elif defined __MVS__
#       define H_OS_ZOS                                                                         1
#   elif defined(__sun) && defined(__SVR4)
#       define H_OS_SOLARIS                                                                     1
#   elif defined(_AIX)
#       define H_OS_AIX                                                                         1
#   elif defined(__hpux)
#       define H_OS_HPUX                                                                        1
#   elif defined __native_client__
#       define H_OS_NACL                                                                        1
#   elif defined __NetBSD__
#       define H_OS_NETBSD                                                                      1
#   elif defined __OpenBSD__
#       define H_OS_OPENBSD                                                                     1
#   elif defined __QNX__
#       define H_OS_QNX                                                                         1
#   elif defined(__HAIKU__)
#       define H_OS_HAIKU                                                                       1
#   elif defined ESP8266
#       define H_OS_ESP8266                                                                     1
#   elif defined ESP32
#       define H_OS_ESP32                                                                       1
#endif  // __CYGWIN__

#endif // __EVENT_ARCH_H__