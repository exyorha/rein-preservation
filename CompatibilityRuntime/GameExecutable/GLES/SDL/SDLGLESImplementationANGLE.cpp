#include <GLES/SDL/SDLGLESImplementationANGLE.h>
#include <GLES/ANGLE/EGLWindowSurface.h>
#include <GLES/ANGLE/InitializedEGLContext.h>

#include <GLES/SDL/RealSDLSymbols.h>

#include <GLES/GLESGlobalSwitches.h>

#include <stdexcept>
#include <vector>

SDLGLESImplementationANGLE::SDLGLESImplementationANGLE() = default;

SDLGLESImplementationANGLE::~SDLGLESImplementationANGLE() = default;

SDL_Window *SDLGLESImplementationANGLE::CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) noexcept {
    flags &= ~SDL_WINDOW_OPENGL;

    return RealSDLSymbols::getSingleton().realCreateWindow(title, x, y, w, h, flags);
}

void SDLGLESImplementationANGLE::DestroyWindowImpl(SDL_Window *window) noexcept {
    auto surface = getSurfaceOfWindow(window);

    if(surface) {
        setSurfaceOfWindow(window, nullptr);
        delete surface;
    }

    RealSDLSymbols::getSingleton().realDestroyWindow(window);
}

std::unique_ptr<BaseGLESContext> SDLGLESImplementationANGLE::CreateContextImpl(SDL_Window *window) {

    printf("SDL_GL_CreateContext called!\n");

    const auto &sdl = RealSDLSymbols::getSingleton();

    SDL_SysWMinfo wmInfo;
    memset(&wmInfo, 0, sizeof(wmInfo));
    wmInfo.version.major = 2;
    wmInfo.version.minor = 0;

    if(!sdl.realGetWindowWMInfo(window, &wmInfo))
        throw std::runtime_error("SDL_GetWindowWMInfo (real) has failed");

    if(!m_display.has_value()) {
        void *nativeDisplay = nullptr;

        std::vector<EGLint> displayAttributes { EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_DEFAULT_ANGLE };

        if(wmInfo.subsystem == SDL_SYSWM_X11) {
            nativeDisplay = wmInfo.info.x11.display;
            displayAttributes.emplace_back(EGL_PLATFORM_ANGLE_NATIVE_PLATFORM_TYPE_ANGLE);
            displayAttributes.emplace_back(EGL_PLATFORM_X11_EXT);

            // TODO: get window visual, stuff it into displayAttributes' EGL_X11_VISUAL_ID_ANGLE, this will avoid a child window
        } else {
            throw std::runtime_error("unsupported SDL WM: " + std::to_string(wmInfo.subsystem));
        }

        displayAttributes.emplace_back(EGL_NONE);

        m_display.emplace(m_angle, nativeDisplay, displayAttributes.data());
        m_displayWM.emplace(wmInfo);
    } else if(!isCompatibleWith(*m_displayWM, wmInfo)) {
        throw std::runtime_error("the already existing EGL display is not compatible with the newly requested window");
    }

    auto config = m_currentAttributes.chooseConfig(m_angle, *m_display);

    auto surface = getSurfaceOfWindow(window);
    if(!surface) {
        std::vector<EGLint> surfaceAttributes;

        surfaceAttributes.emplace_back(EGL_GL_COLORSPACE_KHR);
        if(OpenGLsRGBIsFunctional) {
            surfaceAttributes.emplace_back(EGL_GL_COLORSPACE_SRGB_KHR);
        } else {
            surfaceAttributes.emplace_back(EGL_GL_COLORSPACE_LINEAR_KHR);
        }

        if(m_currentAttributes.attributes[SDL_GL_DOUBLEBUFFER]) {
            surfaceAttributes.emplace_back(EGL_RENDER_BUFFER);
            surfaceAttributes.emplace_back(EGL_BACK_BUFFER);
        } else {
            surfaceAttributes.emplace_back(EGL_RENDER_BUFFER);
            surfaceAttributes.emplace_back(EGL_SINGLE_BUFFER);
        }

        surfaceAttributes.emplace_back(EGL_NONE);

        void *nativeWindow = nullptr;
        if(wmInfo.subsystem == SDL_SYSWM_X11)
            nativeWindow = const_cast<Window *>(&wmInfo.info.x11.window);

        surface = new EGLWindowSurface(m_angle, *m_display, config, nativeWindow, surfaceAttributes.data());
        setSurfaceOfWindow(window, surface);
    }

    EGLContext shareContext = EGL_NO_CONTEXT;

    auto current = currentImplementationContext();
    if(m_currentAttributes.attributes[SDL_GL_SHARE_WITH_CURRENT_CONTEXT] && current) {
        shareContext = *static_cast<InitializedEGLContext *>(current);
    }

    return std::make_unique<InitializedEGLContext>(m_angle, *m_display, config, shareContext);
}

bool SDLGLESImplementationANGLE::isCompatibleWith(const SDL_SysWMinfo &first, const SDL_SysWMinfo &other) {
    if(other.subsystem != first.subsystem)
        return false;

    switch(first.subsystem) {
        case SDL_SYSWM_X11:
            return first.info.x11.display == other.info.x11.display;

        default:
            return false;
    }
}

int SDLGLESImplementationANGLE::MakeCurrentImpl(SDL_Window *window, SDL_GLContext context) noexcept {
    try {
        if(!context) {
            if(!m_angle.eglMakeCurrent(*m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                throw std::runtime_error("EGL_MakeCurrent has failed");
        } else {
            EGLWindowSurface *surface = nullptr;
            if(window) {
                surface = getSurfaceOfWindow(window);
            }

            if(!surface)
                throw std::runtime_error("no window specified or the window doesn't have a surface");


            auto eglContext = static_cast<InitializedEGLContext *>(context);

            if(!m_angle.eglMakeCurrent(*m_display, *surface, *surface, *eglContext))
                throw std::runtime_error("EGL_MakeCurrent has failed");
        }

        return 0;
    } catch(const std::exception &e) {
        fprintf(stderr, "GLESImplementationANGLE::MakeCurrent: failed: %s\n", e.what());
        return -1;
    }
}

int SDLGLESImplementationANGLE::GetSwapInterval() noexcept {
    printf("SDL_GL_GetSwapInterval called!\n");
    abort();
}


int SDLGLESImplementationANGLE::SetSwapInterval(int interval) noexcept {
    printf("SDL_GL_SetSwapInterval(%d) called!\n", interval);
    abort();
}

SDL_bool SDLGLESImplementationANGLE::ExtensionSupported(const char *extension) noexcept {
    printf("SDL_GL_ExtensionSupported called!\n");
    abort();
}

void SDLGLESImplementationANGLE::SwapWindow(SDL_Window *window) noexcept {
    try {
        EGLWindowSurface *surface = nullptr;
        if(window) {
            surface = getSurfaceOfWindow(window);
        }

        if(!surface)
            throw std::runtime_error("no window specified or the window doesn't have a surface");

        if(!m_angle.eglSwapBuffers(*m_display, *surface))
            throw std::runtime_error("EGL_SwapBuffers has failed");

    } catch(const std::exception &e) {
        fprintf(stderr, "GLESImplementationANGLE::SwapBuffers: failed: %s\n", e.what());
    }
}

int SDLGLESImplementationANGLE::GetAttribute(SDL_GLattr attr, int *value) noexcept {
    printf("SDL_GL_GetAttribute called!\n");
    abort();
}

void SDLGLESImplementationANGLE::ResetAttributes() noexcept {
    m_currentAttributes = SDLWrapperAttributeSet();
}

int SDLGLESImplementationANGLE::SetAttribute(SDL_GLattr attr, int value) noexcept {
    if(attr >= m_currentAttributes.attributes.size()) {
        return -1;
    }

    m_currentAttributes.attributes[attr] = value;

    return 0;
}

int SDLGLESImplementationANGLE::LoadLibrary(const char *path) noexcept {
    (void)path;
    return 0;
}

void SDLGLESImplementationANGLE::UnloadLibrary() noexcept {

}

EGLWindowSurface *SDLGLESImplementationANGLE::getSurfaceOfWindow(SDL_Window *window) {
    return static_cast<EGLWindowSurface *>(RealSDLSymbols::getSingleton().realGetWindowData(window, "ANGLEEGLSurface"));
}

void SDLGLESImplementationANGLE::setSurfaceOfWindow(SDL_Window *window, EGLWindowSurface *surface) {
    RealSDLSymbols::getSingleton().realSetWindowData(window, "ANGLEEGLSurface", surface);
}
