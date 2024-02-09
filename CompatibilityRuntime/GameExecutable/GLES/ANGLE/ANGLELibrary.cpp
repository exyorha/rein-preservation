#include <GLES/ANGLE/ANGLELibrary.h>

#include <dlfcn.h>

#include <string>
#include <stdexcept>
#include <sstream>

ANGLELibrary::ANGLELibrary() {
    m_library = dlopen("ANGLE_libGLESv2.so", RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE);
    if(!m_library) {
        throw std::runtime_error("failed to load the ANGLE library: " + std::string(dlerror()));
    }

    eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(dlsym(m_library, "EGL_GetPlatformDisplayEXT"));
    if(!eglGetPlatformDisplayEXT)
        failedToBind("EGL_GetPlatformDisplayEXT");

    eglInitialize = reinterpret_cast<PFNEGLINITIALIZEPROC>(dlsym(m_library, "EGL_Initialize"));
    if(!eglInitialize)
        failedToBind("EGL_Initialize");

    eglTerminate = reinterpret_cast<PFNEGLTERMINATEPROC>(dlsym(m_library, "EGL_Terminate"));
    if(!eglTerminate)
        failedToBind("EGL_Terminate");

    eglBindAPI = reinterpret_cast<PFNEGLBINDAPIPROC>(dlsym(m_library, "EGL_BindAPI"));
    if(!eglBindAPI)
        failedToBind("EGL_BindAPI");

    eglChooseConfig = reinterpret_cast<PFNEGLCHOOSECONFIGPROC>(dlsym(m_library, "EGL_ChooseConfig"));
    if(!eglChooseConfig)
        failedToBind("EGL_ChooseConfig");

    eglCreateContext = reinterpret_cast<PFNEGLCREATECONTEXTPROC>(dlsym(m_library, "EGL_CreateContext"));
    if(!eglCreateContext)
        failedToBind("EGL_CreateContext");

    eglDestroyContext = reinterpret_cast<PFNEGLDESTROYCONTEXTPROC>(dlsym(m_library, "EGL_DestroyContext"));
    if(!eglDestroyContext)
        failedToBind("EGL_DestroyContext");

    eglCreatePlatformWindowSurfaceEXT = reinterpret_cast<PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC>(dlsym(m_library, "EGL_CreatePlatformWindowSurfaceEXT"));
    if(!eglCreatePlatformWindowSurfaceEXT)
        failedToBind("EGL_CreatePlatformWindowSurfaceEXT");

    eglDestroySurface = reinterpret_cast<PFNEGLDESTROYSURFACEPROC>(dlsym(m_library, "EGL_DestroySurface"));
    if(!eglDestroySurface)
        failedToBind("EGL_DestroySurface");

    eglMakeCurrent = reinterpret_cast<PFNEGLMAKECURRENTPROC>(dlsym(m_library, "EGL_MakeCurrent"));
    if(!eglMakeCurrent)
        failedToBind("EGL_MakeCurrent");

    eglSwapBuffers = reinterpret_cast<PFNEGLSWAPBUFFERSPROC>(dlsym(m_library, "EGL_SwapBuffers"));
    if(!eglSwapBuffers)
        failedToBind("EGL_SwapBuffers");
}

void *ANGLELibrary::getProcAddress(const char *name) const {
    return dlsym(m_library, name);
}

void ANGLELibrary::failedToBind(const char *symbol) {
    std::stringstream error;
    error << "Failed to bind a symbol in ANGLE, " << symbol << ": " << dlerror();
    throw std::runtime_error(error.str());
}

ANGLELibrary::~ANGLELibrary() {
    eglTerminate(display);
}
