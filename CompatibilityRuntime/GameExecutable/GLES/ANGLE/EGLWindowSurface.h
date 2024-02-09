#ifndef GLES_ANGLE_EGL_WINDOW_SURFACE_H
#define GLES_ANGLE_EGL_WINDOW_SURFACE_H

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_syswm.h>

#include <GLES/ANGLE/ANGLELibrary.h>

class SDLWrapperAttributeSet;

class EGLWindowSurface {
public:
    EGLWindowSurface(const ANGLELibrary &angle, EGLDisplay display, EGLConfig config, const SDL_SysWMinfo &windowInfo, const SDLWrapperAttributeSet &attributes);
    ~EGLWindowSurface();

    EGLWindowSurface(const EGLWindowSurface &other) = delete;
    EGLWindowSurface &operator =(const EGLWindowSurface &other) = delete;

    static EGLWindowSurface *getSurfaceOfWindow(SDL_Window *window);
    static void setSurfaceOfWindow(SDL_Window *window, EGLWindowSurface *surface);

    inline operator EGLSurface() const {
        return m_surface;
    }

private:
    const ANGLELibrary &m_angle;
    EGLDisplay m_display;
    EGLSurface m_surface;
};

#endif
