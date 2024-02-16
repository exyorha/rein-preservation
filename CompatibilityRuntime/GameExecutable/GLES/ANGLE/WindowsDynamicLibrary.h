#ifndef GLES_ANGLE_WINDOWS_DYNAMIC_LIBRARY_H
#define GLES_ANGLE_WINDOWS_DYNAMIC_LIBRARY_H

#include <windows.h>

class WindowsDynamicLibrary {
public:
    explicit WindowsDynamicLibrary(const wchar_t *name);
    ~WindowsDynamicLibrary();

    WindowsDynamicLibrary(const WindowsDynamicLibrary &other) = delete;
    WindowsDynamicLibrary &operator =(const WindowsDynamicLibrary &other) = delete;

    void *getProcAddress(const char *name) const noexcept;

protected:
    [[noreturn]] static void failedToBind(const char *symbol);

private:
    HMODULE m_module;
};

#endif
