#include <GLES/ANGLE/ANGLELibrary.h>
#include <filesystem>
#include "CommonUtilities.h"

ANGLELibrary::ANGLELibrary() :
#ifdef _WIN32
    WindowsDynamicLibrary(L"ANGLE_libGLESv2.dll")
#else
    LinuxDynamicLibrary(getANGLEPath().c_str(), RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE)
#endif
{

    eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(getProcAddress("EGL_GetPlatformDisplayEXT"));
    if(!eglGetPlatformDisplayEXT)
        failedToBind("EGL_GetPlatformDisplayEXT");

    eglInitialize = reinterpret_cast<PFNEGLINITIALIZEPROC>(getProcAddress("EGL_Initialize"));
    if(!eglInitialize)
        failedToBind("EGL_Initialize");

    eglTerminate = reinterpret_cast<PFNEGLTERMINATEPROC>(getProcAddress("EGL_Terminate"));
    if(!eglTerminate)
        failedToBind("EGL_Terminate");

    eglBindAPI = reinterpret_cast<PFNEGLBINDAPIPROC>(getProcAddress("EGL_BindAPI"));
    if(!eglBindAPI)
        failedToBind("EGL_BindAPI");

    eglChooseConfig = reinterpret_cast<PFNEGLCHOOSECONFIGPROC>(getProcAddress("EGL_ChooseConfig"));
    if(!eglChooseConfig)
        failedToBind("EGL_ChooseConfig");

    eglCreateContext = reinterpret_cast<PFNEGLCREATECONTEXTPROC>(getProcAddress("EGL_CreateContext"));
    if(!eglCreateContext)
        failedToBind("EGL_CreateContext");

    eglDestroyContext = reinterpret_cast<PFNEGLDESTROYCONTEXTPROC>(getProcAddress("EGL_DestroyContext"));
    if(!eglDestroyContext)
        failedToBind("EGL_DestroyContext");

    eglCreatePlatformWindowSurfaceEXT = reinterpret_cast<PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC>(getProcAddress("EGL_CreatePlatformWindowSurfaceEXT"));
    if(!eglCreatePlatformWindowSurfaceEXT)
        failedToBind("EGL_CreatePlatformWindowSurfaceEXT");

    eglDestroySurface = reinterpret_cast<PFNEGLDESTROYSURFACEPROC>(getProcAddress("EGL_DestroySurface"));
    if(!eglDestroySurface)
        failedToBind("EGL_DestroySurface");

    eglMakeCurrent = reinterpret_cast<PFNEGLMAKECURRENTPROC>(getProcAddress("EGL_MakeCurrent"));
    if(!eglMakeCurrent)
        failedToBind("EGL_MakeCurrent");

    eglSwapBuffers = reinterpret_cast<PFNEGLSWAPBUFFERSPROC>(getProcAddress("EGL_SwapBuffers"));
    if(!eglSwapBuffers)
        failedToBind("EGL_SwapBuffers");

    eglSwapInterval = reinterpret_cast<PFNEGLSWAPINTERVALPROC>(getProcAddress("EGL_SwapInterval"));
    if(!eglSwapInterval)
        failedToBind("EGL_SwapInterval");
}

ANGLELibrary::~ANGLELibrary() = default;

#ifndef _WIN32
/*
 * Loading by absolute path under Linux fixes running under RenderDoc.
 */
std::filesystem::path ANGLELibrary::getANGLEPath() {
    return executableDirectory() / "ANGLE_libGLESv2.so";
}
#endif
