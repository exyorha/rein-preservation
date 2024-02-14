#include <GLES/WGL/WGLHooking.h>
#include <GLES/WGL/WindowsImportRedirection.h>

#include <stdexcept>

HGLRC WINAPI replacement_wglGetCurrentContext (void);
HDC WINAPI replacement_wglGetCurrentDC(void);
BOOL WINAPI replacement_wglMakeCurrent(HDC hDc, HGLRC newContext);
HGLRC WINAPI replacement_wglCreateContext(HDC hDc);
HGLRC WINAPI replacement_wglGetCurrentContext(void);
PROC WINAPI replacement_wglGetProcAddress(LPCSTR lpszProc);
BOOL WINAPI replacement_wglDeleteContext(HGLRC oldContext);

static const ReplacementExport replacementWGLFunctions[]{
    { "wglGetCurrentDC",      reinterpret_cast<void *>(replacement_wglGetCurrentDC) },
    { "wglMakeCurrent",       reinterpret_cast<void *>(replacement_wglMakeCurrent) },
    { "wglCreateContext",     reinterpret_cast<void *>(replacement_wglCreateContext) },
    { "wglGetCurrentContext", reinterpret_cast<void *>(replacement_wglGetCurrentContext) },
    { "wglGetProcAddress",    reinterpret_cast<void *>(replacement_wglGetProcAddress) },
    { "wglDeleteContext",     reinterpret_cast<void *>(replacement_wglDeleteContext) },
};

void replaceUnityWGL() {
    auto unityModule = GetModuleHandle(L"UnityPlayer.dll");
    if(!unityModule) {
        throw std::runtime_error("Unable to find UnityPlayer.dll");
    }

    rebindModuleImport(unityModule, "opengl32.dll", replacementWGLFunctions, sizeof(replacementWGLFunctions) / sizeof(replacementWGLFunctions[0]));
}

HDC WINAPI replacement_wglGetCurrentDC(void) {
    printf("wglGetCurrentDC not implemented!\n");
    abort();
    return nullptr;
}

BOOL WINAPI replacement_wglMakeCurrent(HDC hDc, HGLRC newContext) {
    printf("wglMakeCurrent not implemented!\n");
    abort();
    return FALSE;
}

HGLRC WINAPI replacement_wglCreateContext(HDC hDc) {
    printf("wglCreateContext not implemented!\n");
    abort();
    return nullptr;
}

HGLRC WINAPI replacement_wglGetCurrentContext(void) {
    printf("wglGetCurrentContext not implemented!\n");
    abort();
    return nullptr;
}

PROC WINAPI replacement_wglGetProcAddress(LPCSTR lpszProc) {
    printf("wglGetProcAddress not implemented!\n");
    abort();
    return nullptr;
}

BOOL WINAPI replacement_wglDeleteContext(HGLRC oldContext) {
    printf("wglDeleteContext not implemented!\n");
    abort();
    return FALSE;
}
