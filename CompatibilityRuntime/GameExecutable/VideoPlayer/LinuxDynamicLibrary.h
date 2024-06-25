#ifndef VIDEO_PLAYER_LINUX_DYNAMIC_LIBRARY_H
#define VIDEO_PLAYER_LINUX_DYNAMIC_LIBRARY_H

#include <dlfcn.h>

class LinuxDynamicLibrary {
public:
    explicit LinuxDynamicLibrary(const char *name, unsigned int flags = RTLD_NOW | RTLD_LOCAL);
    ~LinuxDynamicLibrary();

    LinuxDynamicLibrary(const LinuxDynamicLibrary &other) = delete;
    LinuxDynamicLibrary &operator =(const LinuxDynamicLibrary &other) = delete;

    void *getProcAddress(const char *name) const noexcept;

private:
    void *m_library;
};

#endif

