#include <GLES/ANGLE/EGLWindowSurface.h>
#include <GLES/ANGLE/SDLWrapperAttributeSet.h>
#include <GLES/RealSDLSymbols.h>

#include <SDL2/SDL_syswm.h>

#include <vector>
#include <stdexcept>


EGLWindowSurface::EGLWindowSurface(const ANGLELibrary &angle, EGLDisplay display, EGLConfig config,
                                   const SDL_SysWMinfo &windowInfo, const SDLWrapperAttributeSet &attributes) :
    m_angle(angle), m_display(display) {

    std::vector<EGLint> surfaceAttributes;

    if(attributes.attributes[SDL_GL_DOUBLEBUFFER]) {
        surfaceAttributes.emplace_back(EGL_RENDER_BUFFER);
        surfaceAttributes.emplace_back(EGL_BACK_BUFFER);
    } else {
        surfaceAttributes.emplace_back(EGL_RENDER_BUFFER);
        surfaceAttributes.emplace_back(EGL_SINGLE_BUFFER);
    }

    surfaceAttributes.emplace_back(EGL_NONE);

    void *nativeWindow = nullptr;
    if(windowInfo.subsystem == SDL_SYSWM_X11)
        nativeWindow = const_cast<Window *>(&windowInfo.info.x11.window);

    m_surface = angle.eglCreatePlatformWindowSurfaceEXT(display, config, nativeWindow, surfaceAttributes.data());
    if(!m_surface)
        throw std::runtime_error("EGL_CreatePlatformWindowSurfaceEXT has failed");
}

EGLWindowSurface::~EGLWindowSurface() {
    m_angle.eglDestroySurface(m_display, m_surface);
}

EGLWindowSurface *EGLWindowSurface::getSurfaceOfWindow(SDL_Window *window) {
    return static_cast<EGLWindowSurface *>(RealSDLSymbols::getSingleton().realGetWindowData(window, "ANGLEEGLSurface"));
}

void EGLWindowSurface::setSurfaceOfWindow(SDL_Window *window, EGLWindowSurface *surface) {
    RealSDLSymbols::getSingleton().realSetWindowData(window, "ANGLEEGLSurface", surface);
}


