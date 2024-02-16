#include <GLES/WGL/ContextAndSurfacePair.h>
#include <GLES/ANGLE/InitializedEGLContext.h>
#include <GLES/ANGLE/EGLWindowSurface.h>

ContextAndSurfacePair::ContextAndSurfacePair(std::unique_ptr<InitializedEGLContext> &&context,
                                             DCAssociatedWindowSurfaceReference &&surface,
                                             HDC associatedDC) : m_context(std::move(context)),
                                             m_surface(std::move(surface)), m_dc(associatedDC) {

}

ContextAndSurfacePair::~ContextAndSurfacePair() = default;

void *ContextAndSurfacePair::getProcAddress(const char *name) noexcept {
    return m_context->getProcAddress(name);
}

EGLContext ContextAndSurfacePair::context() const {
    return *m_context;
}

EGLSurface ContextAndSurfacePair::surface() const {
    return m_surface->surface();
}
