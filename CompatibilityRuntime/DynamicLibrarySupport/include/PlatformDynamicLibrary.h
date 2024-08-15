#ifndef PLATFORM_DYNAMIC_LIBRARY_H
#define PLATFORM_DYNAMIC_LIBRARY_H

#ifdef _WIN32
#include <WindowsDynamicLibrary.h>

using PlatformDynamicLibrary = WindowsDynamicLibrary;
#else
#include <LinuxDynamicLibrary.h>

using PlatformDynamicLibrary = LinuxDynamicLibrary;
#endif

#endif
