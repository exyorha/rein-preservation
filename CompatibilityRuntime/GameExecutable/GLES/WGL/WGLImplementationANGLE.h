#ifndef GLES_WGL_WGL_IMPLEMENTATION_ANGLE_H
#define GLES_WGL_WGL_IMPLEMENTATION_ANGLE_H

#include "GLES/BaseGLESContext.h"
#include <GLES/ANGLE/ANGLELibrary.h>
#include <GLES/ANGLE/InitializedEGLDisplay.h>

#include <GLES/WGL/WGLImplementation.h>

#include <optional>

class WGLImplementationANGLE final : public WGLImplementation {
public:
    WGLImplementationANGLE();
    ~WGLImplementationANGLE();

    const char *GetExtensionsStringARB(HDC hdc) noexcept override;
    BOOL SwapIntervalEXT(int interval) noexcept override;

    BOOL ChoosePixelFormatARB(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,
                              UINT nMaxFormats, int *piFormats, UINT *nNumFormats) noexcept override;

    bool needsSwapBuffersHook() const override;
    WINBOOL SwapBuffers(HDC hdc) noexcept override;

protected:
    std::unique_ptr<BaseGLESContext> CreateContextImpl(HDC hDC) override;
    std::unique_ptr<BaseGLESContext> CreateContextImpl(HDC hDC, BaseGLESContext *hShareContext, const int *attribList) override;

    BOOL MakeCurrentImpl(HDC hDc, BaseGLESContext *newContext) noexcept override;

private:
    std::unique_ptr<BaseGLESContext> CreateContextImpl(HDC hDC, BaseGLESContext *hShareContext);

    ANGLELibrary m_angle;
    InitializedEGLDisplay m_display;
    static const EGLint m_displayAttributes[];
};

#endif
