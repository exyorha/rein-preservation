#include "GLES/WGL/WGLHooking.h"
#include <GLES/WGL/WGLImplementation.h>
#include <GLES/BaseGLESContext.h>
#include <GLES/Shim/GLESContextShim.h>

#include <GL/gl.h>
#include <GL/wgl.h>
#include <GL/wglext.h>

#include <cstdio>
#include <exception>

thread_local HDC WGLImplementation::m_currentDC = nullptr;
thread_local BaseGLESContext *WGLImplementation::m_currentContext = nullptr;

WGLImplementation::WGLImplementation() = default;

WGLImplementation::~WGLImplementation() = default;

HDC WGLImplementation::GetCurrentDC() noexcept {
    return m_currentDC;
}

HGLRC WGLImplementation::GetCurrentContext() noexcept {
    return reinterpret_cast<HGLRC>(m_currentContext);
}

BOOL WGLImplementation::MakeCurrent(HDC hDC, HGLRC newContextHandle) noexcept {
    auto newContext = reinterpret_cast<GLESContextShim *>(newContextHandle);

    if(newContext == nullptr)
        hDC = nullptr;

    if(hDC == m_currentDC && m_currentContext == newContext) {
        SetLastError(ERROR_SUCCESS);
        return true;
    }

    auto result = MakeCurrentImpl(hDC, GLESContextShim::unwrap(newContextHandle));
    if(result) {
        m_currentDC = hDC;
        m_currentContext = newContext;
    } else {
        auto error = GetLastError();

        if(newContext != nullptr) {
            MakeCurrent(nullptr, nullptr);
        }

        SetLastError(error);
    }

    return result;
}

HGLRC WGLImplementation::CreateContext(HDC hDC) noexcept {
    try {
        auto context = CreateContextImpl(hDC);
        if(!context)
            return nullptr;

        SetLastError(ERROR_SUCCESS);

        return reinterpret_cast<HGLRC>(new GLESContextShim(std::move(context)));

    } catch(const std::exception &e) {
        fprintf(stderr, "WGLImplementation::CreateContext: an exception has occurred: %s\n", e.what());

        SetLastError(ERROR_GEN_FAILURE);
        return nullptr;
    }
}

HGLRC WGLImplementation::CreateContext(HDC hDC, HGLRC hShareContext, const int *attribList) noexcept {
    try {
        auto context = CreateContextImpl(hDC, GLESContextShim::unwrap(hShareContext), attribList);
        if(!context)
            return nullptr;

        printf("CreateContext(extended) returning %p\n", context.get());

        SetLastError(ERROR_SUCCESS);

        return reinterpret_cast<HGLRC>(new GLESContextShim(std::move(context)));

    } catch(const std::exception &e) {
        fprintf(stderr, "WGLImplementation::CreateContext: an exception has occurred: %s\n", e.what());

        SetLastError(ERROR_GEN_FAILURE);
        return nullptr;
    }
}

BOOL WGLImplementation::DeleteContext(HGLRC contextHandle) noexcept {
    auto context = reinterpret_cast<GLESContextShim *>(contextHandle);

    if(context == nullptr) {
        SetLastError(ERROR_SUCCESS);

        return TRUE;
    }

    delete context;

    SetLastError(0);

    if(context == m_currentContext) {
        MakeCurrent(nullptr, nullptr);
    }

    return TRUE;
}

PROC WGLImplementation::GetProcAddress(LPCSTR proc) noexcept {
    if(!m_currentContext)
        return nullptr;

    if(!std::string_view(proc).starts_with("wgl"))
        return reinterpret_cast<PROC>(m_currentContext->getProcAddress(proc));

    auto wgl = getExtensionWGLProc(proc);
    if(wgl)
        return reinterpret_cast<PROC>(wgl);

    return GetWGLProcAddress(proc);
}

PROC WGLImplementation::GetWGLProcAddress(LPCSTR proc) noexcept {

    fprintf(stderr, "WGLImplementation::GetProcAddress: the requested extension WGL entry point '%s' was not found. This is not necessarily an error condition.\n", proc);

    return nullptr;
}

const char *WGLImplementation::GetExtensionsStringARB(HDC hdc) noexcept {
    auto extensions = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(GetWGLProcAddress("wglGetExtensionsStringARB"));
    if(!extensions)
        return "";

    auto result = extensions(hdc);

    if(result == nullptr) {
        auto extensions = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(GetWGLProcAddress("wglGetExtensionsStringEXT"));
        if(extensions)
            return extensions();
    }

    return result;
}

BOOL WGLImplementation::SwapIntervalEXT(int interval) noexcept {
    auto proc = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(GetWGLProcAddress("wglSwapIntervalEXT"));
    if(!proc)
        return FALSE;

    return proc(interval);
}

int WGLImplementation::GetSwapIntervalEXT() noexcept {
    auto proc = reinterpret_cast<PFNWGLGETSWAPINTERVALEXTPROC>(GetWGLProcAddress("wglGetSwapIntervalEXT"));
    if(!proc)
        return 1;

    return proc();
}

BOOL WGLImplementation::GetPixelFormatAttribivARB (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes,
                                                   const int *piAttributes, int *piValues) noexcept {

    auto proc = reinterpret_cast<PFNWGLGETPIXELFORMATATTRIBIVARBPROC>(
        GetWGLProcAddress("wglGetPixelFormatAttribivARB")
    );

    if(!proc)
        return FALSE;

    return proc(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues);
}

BOOL WGLImplementation::GetPixelFormatAttribfvARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes,
                                                  const int *piAttributes, FLOAT *pfValues) noexcept {
    auto proc = reinterpret_cast<PFNWGLGETPIXELFORMATATTRIBFVARBPROC>(
        GetWGLProcAddress("wglGetPixelFormatAttribfvARB")
    );

    if(!proc)
        return FALSE;

    return proc(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, pfValues);
}

BOOL WGLImplementation::ChoosePixelFormatARB(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,
                                             UINT nMaxFormats, int *piFormats, UINT *nNumFormats) noexcept {

    auto proc = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(GetWGLProcAddress("wglChoosePixelFormatARB"));

    if(!proc)
        return FALSE;

    return proc(hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats);
}

bool WGLImplementation::needsSwapBuffersHook() const {
    return false;
}

WINBOOL WGLImplementation::SwapBuffers(HDC hdc) noexcept {
    return ::SwapBuffers(hdc);
}
