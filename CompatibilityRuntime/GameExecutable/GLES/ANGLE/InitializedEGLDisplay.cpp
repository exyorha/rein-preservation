#include <GLES/ANGLE/InitializedEGLDisplay.h>

#include <GLES/ANGLE/ANGLELibrary.h>

#include <vector>
#include <stdexcept>
#include <string>

InitializedEGLDisplay::InitializedEGLDisplay(const ANGLELibrary &angle, void *nativeDisplay, const EGLint *attributes) :
    m_angle(angle) {

    m_display = angle.eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, nativeDisplay, attributes);
    if(m_display == EGL_NO_DISPLAY)
        throw std::runtime_error("eglGetPlatformDisplayEXT has failed\n");


    if(!angle.eglInitialize(m_display, nullptr, nullptr))
        throw std::runtime_error("EGL_Initialize has failed");

}

InitializedEGLDisplay::~InitializedEGLDisplay() {
    m_angle.eglTerminate(m_display);
}
