#include "LinuxDynamicLibrary.h"

#include <stdexcept>
#include <sstream>

LinuxDynamicLibrary::LinuxDynamicLibrary(const char *name, unsigned int flags) : m_library(dlopen(name, flags)) {
    if(!m_library) {
        std::stringstream stream;
        stream << "failed to load '" << name << "': " << dlerror();
        throw std::runtime_error(stream.str());
    }
}

LinuxDynamicLibrary::~LinuxDynamicLibrary() {
    dlclose(m_library);
}

void *LinuxDynamicLibrary::getProcAddress(const char *name) const noexcept {
    return dlsym(m_library, name);
}

