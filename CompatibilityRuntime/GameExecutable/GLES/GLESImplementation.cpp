#include <GLES/GLESImplementation.h>
#include <GLES/Shim/GLESContextShim.h>

thread_local SDL_Window *GLESImplementation::m_currentWindow;
thread_local SDL_GLContext GLESImplementation::m_currentContext;

GLESImplementation::GLESImplementation() = default;

GLESImplementation::~GLESImplementation() = default;

void GLESImplementation::DestroyWindow(SDL_Window *window) noexcept {
    DestroyWindowImpl(window);

    if(m_currentWindow == window) {
        MakeCurrent(nullptr, nullptr);
    }
}


SDL_GLContext GLESImplementation::CreateContext(SDL_Window *window) noexcept {
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

void GLESImplementation::DeleteContext(SDL_GLContext context) noexcept {
    if(context) {
        delete static_cast<BaseGLESContext *>(context);

        if(m_currentContext == context) {
            MakeCurrent(nullptr, nullptr);
        }
    }
}

SDL_GLContext GLESImplementation::GetCurrentContext() noexcept {
    return m_currentContext;
}

SDL_Window *GLESImplementation::GetCurrentWindow() noexcept {
    return m_currentWindow;
}

int GLESImplementation::MakeCurrent(SDL_Window *window, SDL_GLContext context) noexcept {
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

BaseGLESContext *GLESImplementation::currentImplementationContext() const noexcept {
    return GLESContextShim::unwrap(m_currentContext);
}

void *GLESImplementation::GetProcAddress(const char *proc) noexcept {
    auto context = static_cast<BaseGLESContext *>(m_currentContext);
    if(!context) {
        fprintf(stderr, "GLESImplementation::GetProcAddress is called with no context set\n");
        return nullptr;
    }

    return context->getProcAddress(proc);
}
