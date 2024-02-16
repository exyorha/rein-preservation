#include <GLES/WGL/WGLHooking.h>
#include <GLES/WGL/WindowsImportRedirection.h>
#include <GLES/WGL/WGLImplementationANGLE.h>
#include <GLES/WGL/WGLImplementationNative.h>

#include <clocale>
#include <stdexcept>
#include <variant>

HGLRC WINAPI replacement_wglGetCurrentContext (void);
HDC WINAPI replacement_wglGetCurrentDC(void);
BOOL WINAPI replacement_wglMakeCurrent(HDC hDc, HGLRC newContext);
HGLRC WINAPI replacement_wglCreateContext(HDC hDc);
HGLRC WINAPI replacement_wglGetCurrentContext(void);
PROC WINAPI replacement_wglGetProcAddress(LPCSTR lpszProc);
BOOL WINAPI replacement_wglDeleteContext(HGLRC oldContext);

const char *WINAPI replacement_wglGetExtensionsStringARB(HDC hdc);
HGLRC WINAPI replacement_wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList);

BOOL WINAPI replacement_wglSwapIntervalEXT(int interval);
int WINAPI replacement_wglGetSwapIntervalEXT(void);

BOOL WINAPI replacement_wglGetPixelFormatAttribivARB (HDC hdc, int iPixelFormat, int iLayerPlane,
                                                      UINT nAttributes, const int *piAttributes, int *piValues);
BOOL WINAPI replacement_wglGetPixelFormatAttribfvARB (HDC hdc, int iPixelFormat, int iLayerPlane,
                                                      UINT nAttributes, const int *piAttributes, FLOAT *pfValues);
BOOL WINAPI replacement_wglChoosePixelFormatARB (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,
                                                 UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

WINBOOL WINAPI replacement_SwapBuffers(HDC hdc);

static std::variant<std::monostate, WGLImplementationNative, WGLImplementationANGLE> WGLImplementationStorage;
static WGLImplementation *SelectedWGLImplementation;

static const ReplacementExport replacementWGLFunctions[]{
    { "wglGetCurrentDC",      reinterpret_cast<void *>(replacement_wglGetCurrentDC) },
    { "wglMakeCurrent",       reinterpret_cast<void *>(replacement_wglMakeCurrent) },
    { "wglCreateContext",     reinterpret_cast<void *>(replacement_wglCreateContext) },
    { "wglGetCurrentContext", reinterpret_cast<void *>(replacement_wglGetCurrentContext) },
    { "wglGetProcAddress",    reinterpret_cast<void *>(replacement_wglGetProcAddress) },
    { "wglDeleteContext",     reinterpret_cast<void *>(replacement_wglDeleteContext) },
};

static const ReplacementExport extensionWGLFunctions[]{
    /*
     * WGL_ARB_extensions_string
     */
    { "wglGetExtensionsStringEXT", reinterpret_cast<void *>(replacement_wglGetExtensionsStringARB) },

    /*
     * WGL_ARB_create_context
     */
    { "wglCreateContextAttribsARB", reinterpret_cast<void *>(replacement_wglCreateContextAttribsARB) },

    /*
     * WGL_EXT_swap_control
     */
    { "wglSwapIntervalEXT",        reinterpret_cast<void *>(replacement_wglSwapIntervalEXT) },
    { "wglGetSwapIntervalEXT",     reinterpret_cast<void *>(replacement_wglGetSwapIntervalEXT) },

    /*
     * WGL_ARB_pixel_format
     */
    { "wglGetPixelFormatAttribivARB", reinterpret_cast<void *>(replacement_wglGetPixelFormatAttribivARB) },
    { "wglGetPixelFormatAttribfvARB", reinterpret_cast<void *>(replacement_wglGetPixelFormatAttribfvARB) },
    { "wglChoosePixelFormatARB",      reinterpret_cast<void *>(replacement_wglChoosePixelFormatARB) }
};

static const ReplacementExport replacementGDIFunctions[]{
    { "SwapBuffers", reinterpret_cast<void *>(replacement_SwapBuffers) }
};

void *getWGLProc(const char *name) {
    return findEntryInReplacementExports(name,
                                         replacementWGLFunctions, sizeof(replacementWGLFunctions) / sizeof(replacementWGLFunctions[0]));
}


void *getExtensionWGLProc(const char *name) {

    /*
     * Make sure that the basic WGL functions are also provided.
     */

    auto proc = getWGLProc(name);
    if(proc)
        return proc;

    return findEntryInReplacementExports(name,
                                         extensionWGLFunctions, sizeof(extensionWGLFunctions) / sizeof(extensionWGLFunctions[0]));
}

void replaceUnityWGL(GLESImplementationType implementationType) {
    printf("Initializing OpenGL ES.\n");

    switch(implementationType) {
        case GLESImplementationType::Native:
            SelectedWGLImplementation = &WGLImplementationStorage.emplace<WGLImplementationNative>();
            break;

        case GLESImplementationType::ANGLE:
            SelectedWGLImplementation = &WGLImplementationStorage.emplace<WGLImplementationANGLE>();
            break;

        default:
            throw std::logic_error("replaceUnityWGL: unsupported implementation type");
    }

    auto unityModule = GetModuleHandle(L"UnityPlayer.dll");
    if(!unityModule) {
        throw std::runtime_error("Unable to find UnityPlayer.dll");
    }

    rebindModuleImport(unityModule, "opengl32.dll", replacementWGLFunctions, sizeof(replacementWGLFunctions) / sizeof(replacementWGLFunctions[0]));

    if(SelectedWGLImplementation->needsSwapBuffersHook()) {
        rebindModuleImport(unityModule, "gdi32.dll", replacementGDIFunctions,
                        sizeof(replacementGDIFunctions) / sizeof(replacementGDIFunctions[0]), true);
    }

    printf("OpenGL ES initialization finished\n");
}

HDC WINAPI replacement_wglGetCurrentDC(void) {
    return SelectedWGLImplementation->GetCurrentDC();
}

BOOL WINAPI replacement_wglMakeCurrent(HDC hDC, HGLRC newContext) {
    return SelectedWGLImplementation->MakeCurrent(hDC, newContext);
}

HGLRC WINAPI replacement_wglCreateContext(HDC hDC) {
    return SelectedWGLImplementation->CreateContext(hDC);
}

HGLRC WINAPI replacement_wglGetCurrentContext(void) {
    return SelectedWGLImplementation->GetCurrentContext();
}

PROC WINAPI replacement_wglGetProcAddress(LPCSTR lpszProc) {
    return SelectedWGLImplementation->GetProcAddress(lpszProc);
}

BOOL WINAPI replacement_wglDeleteContext(HGLRC oldContext) {
    printf("wglDeleteContext\n");
    return SelectedWGLImplementation->DeleteContext(oldContext);
}

const char *WINAPI replacement_wglGetExtensionsStringARB(HDC hdc) {
    return SelectedWGLImplementation->GetExtensionsStringARB(hdc);
}

HGLRC WINAPI replacement_wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList) {
    return SelectedWGLImplementation->CreateContext(hDC, hShareContext, attribList);
}

BOOL WINAPI replacement_wglSwapIntervalEXT(int interval) {
    return SelectedWGLImplementation->SwapIntervalEXT(interval);
}

int WINAPI replacement_wglGetSwapIntervalEXT(void) {
    return SelectedWGLImplementation->GetSwapIntervalEXT();
}

BOOL WINAPI replacement_wglGetPixelFormatAttribivARB (HDC hdc, int iPixelFormat, int iLayerPlane,
                                                      UINT nAttributes, const int *piAttributes, int *piValues) {

    return SelectedWGLImplementation->GetPixelFormatAttribivARB(hdc, iPixelFormat, iLayerPlane, nAttributes,
                                                                piAttributes, piValues);
}

BOOL WINAPI replacement_wglGetPixelFormatAttribfvARB (HDC hdc, int iPixelFormat, int iLayerPlane,
                                                      UINT nAttributes, const int *piAttributes, FLOAT *pfValues) {

    return SelectedWGLImplementation->GetPixelFormatAttribfvARB(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes,
                                                                pfValues);
}

BOOL WINAPI replacement_wglChoosePixelFormatARB (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,
                                                 UINT nMaxFormats, int *piFormats, UINT *nNumFormats) {
    return SelectedWGLImplementation->ChoosePixelFormatARB(hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats,
                                                           nNumFormats);
}

WINBOOL WINAPI replacement_SwapBuffers(HDC hdc) {
    return SelectedWGLImplementation->SwapBuffers(hdc);
}
