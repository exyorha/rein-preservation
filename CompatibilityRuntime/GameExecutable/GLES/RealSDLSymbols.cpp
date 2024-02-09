#include <GLES/RealSDLSymbols.h>

#include <dlfcn.h>

#include <sstream>
#include <stdexcept>

std::optional<RealSDLSymbols> RealSDLSymbols::m_singleton;

RealSDLSymbols::RealSDLSymbols() {
#define BIND_SYMBOL(n) \
    real ## n = reinterpret_cast<SDL_ ## n ## _Type>(dlsym(RTLD_NEXT, "SDL_" #n)); \
    if(!real ## n) \
        failedToBind("SDL_" #n);

    BIND_SYMBOL(CreateWindow)
    BIND_SYMBOL(DestroyWindow)
    BIND_SYMBOL(GetWindowData)
    BIND_SYMBOL(SetWindowData)
    BIND_SYMBOL(GetWindowWMInfo)

    BIND_SYMBOL(GL_CreateContext)
    BIND_SYMBOL(GL_DeleteContext)
    BIND_SYMBOL(GL_ExtensionSupported)
    BIND_SYMBOL(GL_GetAttribute)
    BIND_SYMBOL(GL_GetCurrentContext)
    BIND_SYMBOL(GL_GetCurrentWindow)
    BIND_SYMBOL(GL_GetProcAddress)
    BIND_SYMBOL(GL_GetSwapInterval)
    BIND_SYMBOL(GL_LoadLibrary)
    BIND_SYMBOL(GL_MakeCurrent)
    BIND_SYMBOL(GL_ResetAttributes)
    BIND_SYMBOL(GL_SetAttribute)
    BIND_SYMBOL(GL_SetSwapInterval)
    BIND_SYMBOL(GL_SwapWindow)
    BIND_SYMBOL(GL_UnloadLibrary)

#undef BIND_SYMBOL
}

RealSDLSymbols::~RealSDLSymbols() = default;

void RealSDLSymbols::failedToBind(const char *symbol) {
    std::stringstream error;
    error << "Failed to bind a symbol in UnityPlayer's SDL, " << symbol << ": " << dlerror();
    throw std::runtime_error(error.str());
}

const RealSDLSymbols &RealSDLSymbols::getSingleton() {
    if(!m_singleton.has_value())
        m_singleton.emplace();

    return *m_singleton;
}
