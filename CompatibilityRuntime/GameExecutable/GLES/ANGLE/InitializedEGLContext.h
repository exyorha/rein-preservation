#ifndef GLES_ANGLE_INITIALIZED_EGL_CONTEXT_H
#define GLES_ANGLE_INITIALIZED_EGL_CONTEXT_H

#include <GLES/ANGLE/ANGLELibrary.h>

class InitializedEGLContext {
public:
    InitializedEGLContext(const ANGLELibrary &angle, EGLDisplay display, EGLConfig config, EGLContext shareContext);
    ~InitializedEGLContext();

    InitializedEGLContext(const InitializedEGLContext &other) = delete;
    InitializedEGLContext &operator =(const InitializedEGLContext &other) = delete;

    inline EGLConfig config() const {
        return m_config;
    }

    inline operator EGLContext() const {
        return m_context;
    }

private:
    const ANGLELibrary &m_angle;
    EGLDisplay m_display;
    EGLConfig m_config;
    EGLContext m_context;
};

#endif
