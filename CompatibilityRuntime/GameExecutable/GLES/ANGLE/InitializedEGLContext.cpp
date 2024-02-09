
#include <GLES/ANGLE/InitializedEGLContext.h>

#include <stdexcept>

InitializedEGLContext::InitializedEGLContext(const ANGLELibrary &angle, EGLDisplay display, EGLConfig config, EGLContext shareContext) :
    m_angle(angle),
    m_display(display),
    m_config(config) {

    EGLint attributes[]{
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 0,
        EGL_NONE
    };

    m_context = m_angle.eglCreateContext(display, config, shareContext, attributes);
    if(m_context == EGL_NO_CONTEXT)
        throw std::runtime_error("EGL_CreateContext has failed");
}

InitializedEGLContext::~InitializedEGLContext() {
    m_angle.eglDestroyContext(m_display, m_context);
}
