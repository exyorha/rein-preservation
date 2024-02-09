#include <GLES/ANGLE/GLESImplementationANGLE.h>
#include <GLES/ANGLE/EGLWindowSurface.h>
#include <GLES/ANGLE/InitializedEGLContext.h>

#include <GLES/RealSDLSymbols.h>

#include <stdexcept>

thread_local SDL_Window *GLESImplementationANGLE::m_currentWindow;
thread_local InitializedEGLContext *GLESImplementationANGLE::m_currentContext;

GLESImplementationANGLE::GLESImplementationANGLE() = default;

GLESImplementationANGLE::~GLESImplementationANGLE() = default;

SDL_Window *GLESImplementationANGLE::CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) noexcept {
    flags &= ~SDL_WINDOW_OPENGL;

    return RealSDLSymbols::getSingleton().realCreateWindow(title, x, y, w, h, flags);
}

void GLESImplementationANGLE::DestroyWindow(SDL_Window *window) noexcept {
    auto surface = EGLWindowSurface::getSurfaceOfWindow(window);

    if(surface) {
        EGLWindowSurface::setSurfaceOfWindow(window, nullptr);
        delete surface;
    }

    if(m_currentWindow == window) {
        m_currentWindow = nullptr;
    }

    RealSDLSymbols::getSingleton().realDestroyWindow(window);
}

SDL_GLContext GLESImplementationANGLE::CreateContext(SDL_Window *window) noexcept {

    try {
        printf("SDL_GL_CreateContext called!\n");

        const auto &sdl = RealSDLSymbols::getSingleton();

        SDL_SysWMinfo wmInfo;
        memset(&wmInfo, 0, sizeof(wmInfo));
        wmInfo.version.major = 2;
        wmInfo.version.minor = 0;

        if(!sdl.realGetWindowWMInfo(window, &wmInfo))
            throw std::runtime_error("SDL_GetWindowWMInfo (real) has failed");

        if(!m_display.has_value()) {
            m_display.emplace(m_angle, wmInfo);
        } else if(!m_display->isCompatibleWith(wmInfo)) {
            throw std::runtime_error("the already existing EGL display is not compatible with the newly requested window");
        }

        auto config = m_currentAttributes.chooseConfig(m_angle, *m_display);

        auto surface = EGLWindowSurface::getSurfaceOfWindow(window);
        if(!surface) {
            surface = new EGLWindowSurface(m_angle, *m_display, config, wmInfo, m_currentAttributes);
            EGLWindowSurface::setSurfaceOfWindow(window, surface);
        }

        EGLContext shareContext = EGL_NO_CONTEXT;

        if(m_currentAttributes.attributes[SDL_GL_SHARE_WITH_CURRENT_CONTEXT] && m_currentContext) {
            shareContext = *m_currentContext;
        }

        return new InitializedEGLContext(m_angle, *m_display, config, shareContext);

    } catch(const std::exception &e) {
        fprintf(stderr, "GLESImplementationANGLE::CreateContext: failed: %s\n", e.what());
        return nullptr;
    }
}

void GLESImplementationANGLE::DeleteContext(SDL_GLContext context) noexcept {
    if(context == nullptr)
        return;

    if(context == m_currentContext) {
        m_currentContext = nullptr;
    }

    delete static_cast<InitializedEGLContext *>(context);
}

SDL_GLContext GLESImplementationANGLE::GetCurrentContext() noexcept {
    return m_currentContext;
}

SDL_Window *GLESImplementationANGLE::GetCurrentWindow() noexcept {
    return m_currentWindow;
}

int GLESImplementationANGLE::MakeCurrent(SDL_Window *window, SDL_GLContext context) noexcept {
    try {
        if(!context) {
            window = nullptr;

            if(m_currentContext) {
                m_currentContext = nullptr;
                m_currentWindow = nullptr;

                if(!m_angle.eglMakeCurrent(*m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                    throw std::runtime_error("EGL_MakeCurrent has failed");
            }
        } else {
            EGLWindowSurface *surface = nullptr;
            if(window) {
                surface = EGLWindowSurface::getSurfaceOfWindow(window);
            }

            if(!surface)
                throw std::runtime_error("no window specified or the window doesn't have a surface");


            auto eglContext = static_cast<InitializedEGLContext *>(context);

            if(!m_angle.eglMakeCurrent(*m_display, *surface, *surface, *eglContext))
                throw std::runtime_error("EGL_MakeCurrent has failed");

            m_currentWindow = window;
            m_currentContext = eglContext;
        }

        return 0;
    } catch(const std::exception &e) {
        fprintf(stderr, "GLESImplementationANGLE::MakeCurrent: failed: %s\n", e.what());
        return -1;
    }
}

void *GLESImplementationANGLE::GetProcAddress(const char *proc) noexcept {
    return m_angle.getProcAddress(proc);
}

int GLESImplementationANGLE::GetSwapInterval() noexcept {
    printf("SDL_GL_GetSwapInterval called!\n");
    abort();
}


int GLESImplementationANGLE::SetSwapInterval(int interval) noexcept {
    printf("SDL_GL_SetSwapInterval(%d) called!\n", interval);
    abort();
}

SDL_bool GLESImplementationANGLE::ExtensionSupported(const char *extension) noexcept {
    printf("SDL_GL_ExtensionSupported called!\n");
    abort();
}

void GLESImplementationANGLE::SwapWindow(SDL_Window *window) noexcept {
    try {
        EGLWindowSurface *surface = nullptr;
        if(window) {
            surface = EGLWindowSurface::getSurfaceOfWindow(window);
        }

        if(!surface)
            throw std::runtime_error("no window specified or the window doesn't have a surface");

        if(!m_angle.eglSwapBuffers(*m_display, *surface))
            throw std::runtime_error("EGL_SwapBuffers has failed");

    } catch(const std::exception &e) {
        fprintf(stderr, "GLESImplementationANGLE::SwapBuffers: failed: %s\n", e.what());
    }
}

int GLESImplementationANGLE::GetAttribute(SDL_GLattr attr, int *value) noexcept {
    printf("SDL_GL_GetAttribute called!\n");
    abort();
}

void GLESImplementationANGLE::ResetAttributes() noexcept {
    m_currentAttributes = SDLWrapperAttributeSet();
}

int GLESImplementationANGLE::SetAttribute(SDL_GLattr attr, int value) noexcept {
    if(attr >= m_currentAttributes.attributes.size()) {
        return -1;
    }

    m_currentAttributes.attributes[attr] = value;

    return 0;
}

int GLESImplementationANGLE::LoadLibrary(const char *path) noexcept {
    (void)path;
    return 0;
}

void GLESImplementationANGLE::UnloadLibrary() noexcept {

}

