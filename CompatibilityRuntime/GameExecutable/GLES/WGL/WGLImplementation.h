#ifndef GLES_WGL_WGL_IMPLEMENTATION_H
#define GLES_WGL_WGL_IMPLEMENTATION_H

#include <windows.h>

#include <memory>

class BaseGLESContext;

class WGLImplementation {
protected:
    WGLImplementation();
    ~WGLImplementation();
public:

    WGLImplementation(const WGLImplementation &other) = delete;
    WGLImplementation &operator =(const WGLImplementation &other) = delete;

    HDC GetCurrentDC() noexcept;
    HGLRC GetCurrentContext() noexcept;

    BOOL MakeCurrent(HDC hDc, HGLRC newContext) noexcept;

    HGLRC CreateContext(HDC hDC) noexcept;
    HGLRC CreateContext(HDC hDC, HGLRC hShareContext, const int *attribList) noexcept;
    BOOL DeleteContext(HGLRC context) noexcept;

    PROC GetProcAddress(LPCSTR proc) noexcept;

    /*
     * Extension procedures - if not overriden, will fall back through GetWGLProcAddress.
     */
    virtual const char *GetExtensionsStringARB(HDC hdc) noexcept;
    virtual BOOL SwapIntervalEXT(int interval) noexcept;
    virtual int GetSwapIntervalEXT() noexcept;
    virtual BOOL GetPixelFormatAttribivARB (HDC hdc, int iPixelFormat, int iLayerPlane,
                                            UINT nAttributes, const int *piAttributes, int *piValues) noexcept;
    virtual BOOL GetPixelFormatAttribfvARB(HDC hdc, int iPixelFormat, int iLayerPlane,
                                           UINT nAttributes, const int *piAttributes, FLOAT *pfValues) noexcept;
    virtual BOOL ChoosePixelFormatARB(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,
                                      UINT nMaxFormats, int *piFormats, UINT *nNumFormats) noexcept;

    virtual bool needsSwapBuffersHook() const;
    virtual WINBOOL SwapBuffers(HDC hdc) noexcept;

protected:
    virtual std::unique_ptr<BaseGLESContext> CreateContextImpl(HDC hDC) = 0;
    virtual std::unique_ptr<BaseGLESContext> CreateContextImpl(HDC hDC, BaseGLESContext *hShareContext,
                                                               const int *attribList) = 0;
    virtual BOOL MakeCurrentImpl(HDC hDc, BaseGLESContext *newContext) noexcept = 0;

    virtual PROC GetWGLProcAddress(LPCSTR proc) noexcept;

private:
    static thread_local HDC m_currentDC;
    static thread_local BaseGLESContext *m_currentContext;

};

#endif
