#ifndef ZENFIRE_EXAMPLES_COMPAT_HPP_INCLUDED
#define ZENFIRE_EXAMPLES_COMPAT_HPP_INCLUDED

// compat
#if defined(_MSC_VER)
#  include "windows.h"
#  define COMPAT_SLEEP(a) Sleep((a * 1000))
#  define COMPAT_MSLEEP(a) Sleep(a)
#else
#  define COMPAT_SLEEP(a) sleep(a)
#  define COMPAT_MSLEEP(a) usleep(a * 1000)
#endif

#endif
