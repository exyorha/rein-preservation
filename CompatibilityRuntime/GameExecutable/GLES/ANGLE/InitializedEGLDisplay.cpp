#include <GLES/ANGLE/InitializedEGLDisplay.h>

#include <GLES/ANGLE/ANGLELibrary.h>

#include <vector>
#include <stdexcept>
#include <string>

InitializedEGLDisplay::InitializedEGLDisplay(const ANGLELibrary &angle, const SDL_SysWMinfo &wmInfo) : m_angle(angle), m_initialWMInfo(wmInfo) {

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

    m_display = angle.eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, nativeDisplay, displayAttributes.data());
    if(m_display == EGL_NO_DISPLAY)
        throw std::runtime_error("eglGetPlatformDisplayEXT has failed\n");


    if(!angle.eglInitialize(m_display, nullptr, nullptr))
        throw std::runtime_error("EGL_Initialize has failed");

}

InitializedEGLDisplay::~InitializedEGLDisplay() {
    m_angle.eglTerminate(m_display);
}

bool InitializedEGLDisplay::isCompatibleWith(const SDL_SysWMinfo &other) const {
    if(other.subsystem != m_initialWMInfo.subsystem)
        return false;

    switch(m_initialWMInfo.subsystem) {
        case SDL_SYSWM_X11:
            return m_initialWMInfo.info.x11.display == other.info.x11.display;

        default:
            return false;
    }
}
