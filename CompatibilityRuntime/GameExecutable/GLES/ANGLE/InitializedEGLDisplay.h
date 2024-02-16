#ifndef GLES_ANGLE_INITIALIZED_EGL_DISPLAY_H
#define GLES_ANGLE_INITIALIZED_EGL_DISPLAY_H

#include <GLES/ANGLE/ANGLELibrary.h>

class InitializedEGLDisplay {
public:
    explicit InitializedEGLDisplay(const ANGLELibrary &angle, void *nativeDisplay = nullptr, const EGLint *attributes = nullptr);
    ~InitializedEGLDisplay();

    InitializedEGLDisplay(const InitializedEGLDisplay &other) = delete;
    InitializedEGLDisplay &operator =(const InitializedEGLDisplay &other) = delete;

    inline operator EGLDisplay() const {
        return m_display;
    }

private:
    const ANGLELibrary &m_angle;
    EGLDisplay m_display;
};

#endif
