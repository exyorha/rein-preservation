#ifndef GLES_ANGLE_ANGLE_LIBRARY_H
#define GLES_ANGLE_ANGLE_LIBRARY_H

#define EGL_EGL_PROTOTYPES 0
#define EGL_EGLEXT_PROTOTYPES 0
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglext_angle.h>

#include <GLES/ANGLE/PlatformDynamicLibrary.h>

#include <filesystem>

class ANGLELibrary final : public PlatformDynamicLibrary {
public:
    ANGLELibrary();
    ~ANGLELibrary();

    ANGLELibrary(const ANGLELibrary &other) = delete;
    ANGLELibrary &operator =(const ANGLELibrary &other) = delete;

    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT;
    PFNEGLINITIALIZEPROC eglInitialize;
    PFNEGLTERMINATEPROC eglTerminate;
    PFNEGLBINDAPIPROC eglBindAPI;

    PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC eglCreatePlatformWindowSurfaceEXT;
    PFNEGLDESTROYSURFACEPROC eglDestroySurface;

    PFNEGLCHOOSECONFIGPROC eglChooseConfig;

    PFNEGLCREATECONTEXTPROC eglCreateContext;
    PFNEGLDESTROYCONTEXTPROC eglDestroyContext;

    PFNEGLMAKECURRENTPROC eglMakeCurrent;

    PFNEGLSWAPBUFFERSPROC eglSwapBuffers;
    PFNEGLSWAPINTERVALPROC eglSwapInterval;

private:
#ifndef _WIN32
    static std::filesystem::path getANGLEPath();
#endif
};

#endif
