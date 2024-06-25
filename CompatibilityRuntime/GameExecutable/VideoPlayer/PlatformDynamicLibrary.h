#ifndef VIDEO_PLAYER_PLATFORM_DYNAMIC_LIBRARY_H
#define VIDEO_PLAYER_PLATFORM_DYNAMIC_LIBRARY_H

#ifdef _WIN32
#include <VideoPlayer/WindowsDynamicLibrary.h>

using PlatformDynamicLibrary = WindowsDynamicLibrary;
#else
#include <VideoPlayer/LinuxDynamicLibrary.h>

using PlatformDynamicLibrary = LinuxDynamicLibrary;
#endif

#endif
