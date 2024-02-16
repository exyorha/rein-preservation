#include <GLES/WGL/WGLImplementationNative.h>

#include <GL/gl.h>
#include <GL/wgl.h>
#include <GL/wglext.h>

#include <GL/gl.h>

WGLImplementationNative::WGLImplementationNative() = default;

WGLImplementationNative::~WGLImplementationNative() = default;

std::unique_ptr<BaseGLESContext> WGLImplementationNative::CreateContextImpl(HDC hDC) {

    auto context = wglCreateContext(hDC);
    if(!context)
        return nullptr;

    return std::make_unique<NativeGLESContext>(context);
}

std::unique_ptr<BaseGLESContext> WGLImplementationNative::CreateContextImpl(HDC hDC, BaseGLESContext *hShareContext,
                                                            const int *attribList) {

    auto proc = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
    if(!proc) {
        fprintf(stderr, "WGLImplementationNative::CreateContextImpl: the native implementation cannot provide wglCreateContextAttribsARB\n");

        return nullptr;
    }

    HGLRC shareHandle = nullptr;
    if(hShareContext) {
        shareHandle = static_cast<NativeGLESContext *>(hShareContext)->context();
    }

    auto context = proc(hDC, shareHandle, attribList);
    if(!context)
        return nullptr;

    return std::make_unique<NativeGLESContext>(context);
}

BOOL WGLImplementationNative::MakeCurrentImpl(HDC hDc, BaseGLESContext *newContext) noexcept {
    HGLRC nativeTargetContext = nullptr;

    if(newContext) {
        nativeTargetContext = static_cast<NativeGLESContext *>(newContext)->context();
    }

    return wglMakeCurrent(hDc, nativeTargetContext);
}

PROC WGLImplementationNative::GetWGLProcAddress(LPCSTR procname) noexcept {
    return GetWGLProcAddressStatic(procname);
}

PROC WGLImplementationNative::GetWGLProcAddressStatic(LPCSTR procname) noexcept {
    auto proc = wglGetProcAddress(procname);
    if(!proc) {
        auto handle = GetModuleHandle(L"opengl32.dll");
        if(handle)
            return ::GetProcAddress(handle, procname);
    }

    return proc;
}

WGLImplementationNative::NativeGLESContext::NativeGLESContext(HGLRC context) : m_context(context) {

}

WGLImplementationNative::NativeGLESContext::~NativeGLESContext() {
    wglDeleteContext(m_context);
}

void *WGLImplementationNative::NativeGLESContext::getProcAddress(const char *name) noexcept {
    return reinterpret_cast<void *>(WGLImplementationNative::GetWGLProcAddressStatic(name));
}
