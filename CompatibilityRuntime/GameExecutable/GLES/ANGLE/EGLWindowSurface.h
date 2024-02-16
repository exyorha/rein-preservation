#ifndef GLES_ANGLE_EGL_WINDOW_SURFACE_H
#define GLES_ANGLE_EGL_WINDOW_SURFACE_H

#include <GLES/ANGLE/ANGLELibrary.h>

class SDLWrapperAttributeSet;

class EGLWindowSurface {
public:
    EGLWindowSurface(const ANGLELibrary &angle, EGLDisplay display, EGLConfig config, void *nativeWindow,
                     const EGLint *attributes);
    ~EGLWindowSurface();

    EGLWindowSurface(const EGLWindowSurface &other) = delete;
    EGLWindowSurface &operator =(const EGLWindowSurface &other) = delete;

    inline operator EGLSurface() const {
        return m_surface;
    }

private:
    const ANGLELibrary &m_angle;
    EGLDisplay m_display;
    EGLSurface m_surface;
};

#endif
