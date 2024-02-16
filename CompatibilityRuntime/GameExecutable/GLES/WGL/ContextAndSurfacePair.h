#ifndef GLES_WGL_CONTEXT_AND_SURFACE_PAIR_H
#define GLES_WGL_CONTEXT_AND_SURFACE_PAIR_H

#include <GLES/ANGLE/ANGLELibrary.h>

#include <GLES/WGL/DCAssociatedWindowSurface.h>

#include <GLES/BaseGLESContext.h>

#include <windows.h>

#include <memory>

class InitializedEGLContext;
class EGLWindowSurface;

class ContextAndSurfacePair final : public BaseGLESContext {
public:
    explicit ContextAndSurfacePair(
        std::unique_ptr<InitializedEGLContext> &&context,
        DCAssociatedWindowSurfaceReference &&surface,
        HDC associatedDC);
    ~ContextAndSurfacePair() override;

    void *getProcAddress(const char *name) noexcept override;

    inline HDC dc() const {
        return m_dc;
    }

    EGLContext context() const;
    EGLSurface surface() const;

private:
    std::unique_ptr<InitializedEGLContext> m_context;
    DCAssociatedWindowSurfaceReference m_surface;
    HDC m_dc;
};

#endif
