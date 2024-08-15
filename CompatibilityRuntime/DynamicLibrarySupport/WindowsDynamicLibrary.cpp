#include "WindowsDynamicLibrary.h"

#include <stdexcept>
#include <sstream>

WindowsDynamicLibrary::WindowsDynamicLibrary(const wchar_t *name) {
    m_module = LoadLibrary(name);

    if(!m_module)
        throw std::runtime_error("LoadLibrary has failed");
}

WindowsDynamicLibrary::~WindowsDynamicLibrary() {
    FreeLibrary(m_module);
}

void *WindowsDynamicLibrary::getProcAddress(const char *name) const noexcept {
    return reinterpret_cast<void *>(GetProcAddress(m_module, name));
}

