#include <GLES/ANGLE/EGLWindowSurface.h>


#include <vector>
#include <stdexcept>


EGLWindowSurface::EGLWindowSurface(const ANGLELibrary &angle, EGLDisplay display, EGLConfig config,
                                   void *nativeWindow, const EGLint *surfaceAttributes) :
    m_angle(angle), m_display(display) {


    m_surface = angle.eglCreatePlatformWindowSurfaceEXT(display, config, nativeWindow, surfaceAttributes);
    if(!m_surface)
        throw std::runtime_error("EGL_CreatePlatformWindowSurfaceEXT has failed");
}

EGLWindowSurface::~EGLWindowSurface() {
    m_angle.eglDestroySurface(m_display, m_surface);
}
