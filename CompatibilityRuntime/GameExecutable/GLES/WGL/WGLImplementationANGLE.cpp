#include <GLES/WGL/WGLImplementationANGLE.h>
#include <GLES/WGL/ContextAndSurfacePair.h>
#include <GLES/WGL/WGLHooking.h>
#include <GLES/WGL/WindowsImportRedirection.h>
#include <GLES/WGL/DCAssociatedWindowSurface.h>

#include <GLES/BaseGLESContext.h>

#include <GLES/ANGLE/EGLWindowSurface.h>
#include <GLES/ANGLE/InitializedEGLContext.h>

#include <GL/gl.h>
#include <GL/wglext.h>

#include <vector>

const EGLint WGLImplementationANGLE::m_displayAttributes[]{
    EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
    EGL_EXPERIMENTAL_PRESENT_PATH_ANGLE, EGL_EXPERIMENTAL_PRESENT_PATH_FAST_ANGLE,
    EGL_NONE
};

WGLImplementationANGLE::WGLImplementationANGLE() : m_display(m_angle, EGL_DEFAULT_DISPLAY, m_displayAttributes) {

}

WGLImplementationANGLE::~WGLImplementationANGLE() = default;

/*
 * WGL API allows the user to freely switch DCs on a single context. For
 * simplification, we don't implement that, since Unity does not need it.
 */
std::unique_ptr<BaseGLESContext> WGLImplementationANGLE::CreateContextImpl(HDC hDC) {
    return CreateContextImpl(hDC, nullptr);
}

std::unique_ptr<BaseGLESContext> WGLImplementationANGLE::CreateContextImpl(HDC hDC, BaseGLESContext *hShareContext,
                                                                           const int *attribList) {

    int profileMask = 0;
    int majorVersion = 0;
    int minorVersion = 0;
    int contextFlags = 0;

    if(attribList) {
        for(auto pair = attribList; *pair != 0; pair += 2) {
            auto token = pair[0];
            auto value = pair[1];

            if(token == WGL_CONTEXT_MAJOR_VERSION_ARB) {
                majorVersion = value;
            } else if(token == WGL_CONTEXT_MINOR_VERSION_ARB) {
                minorVersion = value;
            } else if(token == WGL_CONTEXT_FLAGS_ARB) {
                contextFlags = value;
            } else if(token == WGL_CONTEXT_PROFILE_MASK_ARB) {
                profileMask = value;
            } else {

                printf("WGLImplementationANGLE::CreateContextImpl: attribute: 0x%04X = %d\n",
                    token, value);
            }
        }
    }

    if(profileMask != 4) {
        fprintf(stderr, "WGLImplementationANGLE::CreateContextImpl: requesting something other than an OpenGL ES context (profile mask %d)\n", profileMask);
        return nullptr;
    }

    printf("WGLImplementationANGLE::CreateContextImpl: requesting a version %d.%d context, with context flags %d\n",
           majorVersion, minorVersion, contextFlags);

    if(majorVersion > 3 || (majorVersion == 3 && minorVersion > 0)) {
        fprintf(stderr, "WGLImplementationANGLE::CreateContextImpl: requesting an OpenGL ES context of too high of a version %d.%d\n", majorVersion, minorVersion);
        return nullptr;
    }

    return CreateContextImpl(hDC, hShareContext);
}

std::unique_ptr<BaseGLESContext> WGLImplementationANGLE::CreateContextImpl(HDC hDC, BaseGLESContext *hShareContext) {

    if(!hDC) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return nullptr;
    }

    auto existingSurface = DCAssociatedWindowSurface::getSurfaceOfDC(hDC);

    if(!existingSurface) {
        existingSurface = DCAssociatedWindowSurfaceReference(new DCAssociatedWindowSurface(
            m_angle, m_display, hDC));
    }


    EGLContext shareWithEGLContext = nullptr;

    if(hShareContext) {
        shareWithEGLContext = static_cast<ContextAndSurfacePair *>(hShareContext)->context();
    }

    auto context = std::make_unique<InitializedEGLContext>(
        m_angle, m_display, existingSurface->config(), shareWithEGLContext);

    return std::make_unique<ContextAndSurfacePair>(std::move(context),
                                                   DCAssociatedWindowSurfaceReference(existingSurface), hDC);
}

BOOL WGLImplementationANGLE::MakeCurrentImpl(HDC hDc, BaseGLESContext *newContext) noexcept  {
    if(!newContext) {
        m_angle.eglMakeCurrent(m_display, nullptr, nullptr, nullptr);
        return TRUE;
    }

    auto pair = static_cast<ContextAndSurfacePair *>(newContext);
    if(pair->dc() != hDc) {
        fprintf(stderr, "WGLImplementationANGLE::MakeCurrentImpl: attempted to set a DC different from the one the context was created with. It is assumed that this functionality is not used. The call will fail.\n");
        return FALSE;
    }

    m_angle.eglMakeCurrent(m_display, pair->surface(), pair->surface(), pair->context());

    return TRUE;
}

const char *WGLImplementationANGLE::GetExtensionsStringARB(HDC hdc) noexcept {
    (void)hdc;

    return "WGL_ARB_extensions_string,WGL_ARB_create_context,WGL_ARB_create_context_profile,WGL_EXT_create_context_es_profile,WGL_EXT_create_context_es2_profile,WGL_EXT_swap_control,WGL_EXT_colorspace,WGL_ARB_framebuffer_sRGB,WGL_EXT_framebuffer_sRGB,WGL_ARB_pixel_format";
}

BOOL WGLImplementationANGLE::SwapIntervalEXT(int interval) noexcept {
    if(m_angle.eglSwapInterval(m_display, interval))
        return TRUE;
    else
        return FALSE;
}

BOOL WGLImplementationANGLE::ChoosePixelFormatARB(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,
                                                  UINT nMaxFormats, int *piFormats, UINT *nNumFormats) noexcept {


    /*
     * This is sufficient to make Unity revert to the non-extension ChoosePixelFormat.
     */

    if(nMaxFormats > 0) {
        *piFormats = 0;
    }

    *nNumFormats = 0;

    return FALSE;
}

bool WGLImplementationANGLE::needsSwapBuffersHook() const {
    return true;
}

WINBOOL WGLImplementationANGLE::SwapBuffers(HDC hdc) noexcept {
    (void)hdc;

    auto surface = DCAssociatedWindowSurface::getSurfaceOfDC(hdc);
    if(!surface)
        return ::SwapBuffers(hdc);

    if(m_angle.eglSwapBuffers(m_display, surface->surface())) {
        return TRUE;
    } else {
        return FALSE;
    }
}
