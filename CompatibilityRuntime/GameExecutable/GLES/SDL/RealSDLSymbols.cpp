#include <GLES/SDL/RealSDLSymbols.h>

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
    BIND_SYMBOL(GetWindowWMInfo)
    BIND_SYMBOL(GetWindowSize)

    BIND_SYMBOL(GL_GetProcAddress)
    BIND_SYMBOL(GL_SwapWindow)


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
