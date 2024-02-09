#ifndef GLES_ANGLE_INITIALIZED_EGL_DISPLAY_H
#define GLES_ANGLE_INITIALIZED_EGL_DISPLAY_H

#include <SDL2/SDL_syswm.h>

#include <GLES/ANGLE/ANGLELibrary.h>


class InitializedEGLDisplay {
public:
    explicit InitializedEGLDisplay(const ANGLELibrary &angle, const SDL_SysWMinfo &wmInfo);
    ~InitializedEGLDisplay();

    InitializedEGLDisplay(const InitializedEGLDisplay &other) = delete;
    InitializedEGLDisplay &operator =(const InitializedEGLDisplay &other) = delete;

    bool isCompatibleWith(const SDL_SysWMinfo &other) const;

    inline operator EGLDisplay() const {
        return m_display;
    }

private:
    const ANGLELibrary &m_angle;
    SDL_SysWMinfo m_initialWMInfo;
    EGLDisplay m_display;
};

#endif
