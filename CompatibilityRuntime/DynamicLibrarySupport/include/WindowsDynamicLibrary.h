#ifndef WINDOWS_DYNAMIC_LIBRARY_H
#define WINDOWS_DYNAMIC_LIBRARY_H

#include <windows.h>

class WindowsDynamicLibrary {
public:
    explicit WindowsDynamicLibrary(const wchar_t *name);
    ~WindowsDynamicLibrary();

    WindowsDynamicLibrary(const WindowsDynamicLibrary &other) = delete;
    WindowsDynamicLibrary &operator =(const WindowsDynamicLibrary &other) = delete;

    void *getProcAddress(const char *name) const noexcept;

private:
    HMODULE m_module;
};

#endif
