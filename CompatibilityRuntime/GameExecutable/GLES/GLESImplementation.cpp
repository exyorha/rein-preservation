#include <GLES/GLESImplementation.h>

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

void GLESImplementation::DeleteContext(SDL_GLContext context) noexcept {
    DeleteContextImpl(context);

    if(m_currentContext == context) {
        MakeCurrent(nullptr, nullptr);
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

    auto result = MakeCurrentImpl(window, context);
    if(result == 0) {
        m_currentWindow = window;
        m_currentContext = context;
    }

    return result;
}
