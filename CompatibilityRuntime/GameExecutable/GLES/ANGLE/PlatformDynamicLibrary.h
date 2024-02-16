#ifndef GLES_ANGLE_PLATFORM_DYNAMIC_LIBRARY_H
#define GLES_ANGLE_PLATFORM_DYNAMIC_LIBRARY_H

#ifdef _WIN32
#include <GLES/ANGLE/WindowsDynamicLibrary.h>

using PlatformDynamicLibrary = WindowsDynamicLibrary;
#else
#include <GLES/ANGLE/LinuxDynamicLibrary.h>

using PlatformDynamicLibrary = LinuxDynamicLibrary;
#endif

#endif
