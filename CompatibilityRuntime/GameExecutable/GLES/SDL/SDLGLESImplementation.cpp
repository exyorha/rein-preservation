#include <GLES/SDL/SDLGLESImplementation.h>
#include <GLES/Shim/GLESContextShim.h>

thread_local SDL_Window *SDLGLESImplementation::m_currentWindow;
thread_local SDL_GLContext SDLGLESImplementation::m_currentContext;

SDLGLESImplementation::SDLGLESImplementation() = default;

SDLGLESImplementation::~SDLGLESImplementation() = default;

void SDLGLESImplementation::DestroyWindow(SDL_Window *window) noexcept {
    DestroyWindowImpl(window);

    if(m_currentWindow == window) {
        MakeCurrent(nullptr, nullptr);
    }
}


SDL_GLContext SDLGLESImplementation::CreateContext(SDL_Window *window) noexcept {
    try {
        auto context = CreateContextImpl(window);
        if(!context) {
            return nullptr;
        }

        auto shim = std::make_unique<GLESContextShim>(std::move(context));

        return shim.release();

    } catch(const std::exception &e) {
        fprintf(stderr, "GLESImplementation::CreateContext: failed: %s\n", e.what());
        return nullptr;
    }
}

void SDLGLESImplementation::DeleteContext(SDL_GLContext context) noexcept {
    if(context) {
        delete static_cast<BaseGLESContext *>(context);

        if(m_currentContext == context) {
            MakeCurrent(nullptr, nullptr);
        }
    }
}

SDL_GLContext SDLGLESImplementation::GetCurrentContext() noexcept {
    return m_currentContext;
}

SDL_Window *SDLGLESImplementation::GetCurrentWindow() noexcept {
    return m_currentWindow;
}

int SDLGLESImplementation::MakeCurrent(SDL_Window *window, SDL_GLContext context) noexcept {
    if(context == nullptr)
        window = nullptr;

    if(window == m_currentWindow && context == m_currentContext)
        return 0;

    auto result = MakeCurrentImpl(window, GLESContextShim::unwrap(context));
    if(result == 0) {
        m_currentWindow = window;
        m_currentContext = context;
    }

    return result;
}

BaseGLESContext *SDLGLESImplementation::currentImplementationContext() const noexcept {
    return GLESContextShim::unwrap(m_currentContext);
}

void *SDLGLESImplementation::GetProcAddress(const char *proc) noexcept {
    auto context = static_cast<BaseGLESContext *>(m_currentContext);
    if(!context) {
        fprintf(stderr, "GLESImplementation::GetProcAddress is called with no context set\n");
        return nullptr;
    }

    return context->getProcAddress(proc);
}
