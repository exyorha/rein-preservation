#include <GLES/WGL/DCAssociatedWindowSurface.h>
#include <GLES/ANGLE/EGLWindowSurface.h>

#include <stdexcept>
#include <vector>

DCAssociatedWindowSurface::DCAssociatedWindowSurface(const ANGLELibrary &angle,
    EGLDisplay display, HDC hDC) : m_references(1),
    m_association(hDC, this) {

    auto window = WindowFromDC(hDC);
    if(!window)
        throw std::runtime_error("WindowFromDC has failed");

    auto pixelFormatIndex = GetPixelFormat(hDC);
    if(pixelFormatIndex == 0)
        throw std::runtime_error("GetPixelFormat has failed");

    PIXELFORMATDESCRIPTOR formatDescriptor;
    if(!DescribePixelFormat(hDC, pixelFormatIndex, sizeof(formatDescriptor), &formatDescriptor))
        throw std::runtime_error("DescribePixelFormat has failed");

    printf("DCAssociatedWindowSurface: hDC = %p, window = %p, selected pixel format = %d, flags = %u, red bits: %d, green bits: %d, blue bits: %d, depth bits: %d, stencil bits: %d\n",
           hDC, window, pixelFormatIndex, formatDescriptor.dwFlags, formatDescriptor.cRedBits, formatDescriptor.cGreenBits, formatDescriptor.cBlueBits,
           formatDescriptor.cDepthBits, formatDescriptor.cStencilBits);

    /*
     * NOTE: basic wglCreateContext doesn't support MSAA.
     */
    std::vector<EGLint> configAttributes{
        EGL_RED_SIZE, formatDescriptor.cRedBits,
        EGL_GREEN_SIZE, formatDescriptor.cGreenBits,
        EGL_BLUE_SIZE, formatDescriptor.cBlueBits,
        EGL_ALPHA_SIZE, formatDescriptor.cAlphaBits,
        EGL_DEPTH_SIZE, formatDescriptor.cDepthBits,
        EGL_STENCIL_SIZE, formatDescriptor.cStencilBits,
        EGL_NONE
    };

    int availableConfigs;
    if(!angle.eglChooseConfig(display, configAttributes.data(), &m_config, 1, &availableConfigs) ||
        availableConfigs == 0)
        throw std::runtime_error("EGL_ChooseConfig has failed");

    printf("config chosen: %p\n", m_config);

    std::vector<EGLint> surfaceAttributes;
    if(formatDescriptor.dwFlags & PFD_DOUBLEBUFFER) {
        surfaceAttributes.emplace_back(EGL_RENDER_BUFFER);
        surfaceAttributes.emplace_back(EGL_BACK_BUFFER);
    } else {
        surfaceAttributes.emplace_back(EGL_RENDER_BUFFER);
        surfaceAttributes.emplace_back(EGL_SINGLE_BUFFER);
    }

    surfaceAttributes.push_back(EGL_NONE);

    m_surface = std::make_unique<EGLWindowSurface>(
        angle, display, m_config, window, surfaceAttributes.data());

    printf("surface created: %p\n", m_surface.get());

}

DCAssociatedWindowSurface::~DCAssociatedWindowSurface() {
    printf("destroying surface: %p\n", m_surface.get());
}

void DCAssociatedWindowSurface::addReference() noexcept {
    m_references.fetch_add(1);
}

void DCAssociatedWindowSurface::release() noexcept {
    if(m_references.fetch_sub(1) == 1)
        delete this;
}

EGLSurface DCAssociatedWindowSurface::surface() const {
    return *m_surface;
}

DCAssociatedWindowSurfaceReference DCAssociatedWindowSurface::getSurfaceOfDC(HDC dc) noexcept {
    auto surface = getSurfaceOfWindow(WindowFromDC(dc));
    if(surface) {
        surface->addReference();
    }

    return DCAssociatedWindowSurfaceReference(surface);
}

DCAssociatedWindowSurface *DCAssociatedWindowSurface::getSurfaceOfWindow(HWND window) noexcept {
    return reinterpret_cast<DCAssociatedWindowSurface *>(GetProp(window, L"WGLANGLEWindowSurface"));
}

void DCAssociatedWindowSurface::setSurfaceOfDC(HDC dc, DCAssociatedWindowSurface *surface) {
    setSurfaceOfWindow(WindowFromDC(dc), surface);
}

void DCAssociatedWindowSurface::setSurfaceOfWindow(HWND window, DCAssociatedWindowSurface *surface) {
    if(surface == nullptr) {
        RemoveProp(window, L"WGLANGLEWindowSurface");
    } else {
        if(!SetProp(window, L"WGLANGLEWindowSurface", reinterpret_cast<HANDLE>(surface)))
            throw std::runtime_error("SetProp has failed");
    }
}

DCAssociatedWindowSurface::Association::Association(HDC hDC, DCAssociatedWindowSurface *surface) : m_dc(hDC) {
    DCAssociatedWindowSurface::setSurfaceOfDC(hDC, surface);
}

DCAssociatedWindowSurface::Association::~Association() {
    DCAssociatedWindowSurface::setSurfaceOfDC(m_dc, nullptr);
}

DCAssociatedWindowSurfaceReference::DCAssociatedWindowSurfaceReference(DCAssociatedWindowSurface *surface) noexcept : m_surface(surface) {

}

DCAssociatedWindowSurfaceReference::~DCAssociatedWindowSurfaceReference() {
    if(m_surface)
        m_surface->release();
}

DCAssociatedWindowSurfaceReference::DCAssociatedWindowSurfaceReference(const DCAssociatedWindowSurfaceReference &other) noexcept : DCAssociatedWindowSurfaceReference() {
    *this = other;
}

DCAssociatedWindowSurfaceReference &DCAssociatedWindowSurfaceReference::operator =(const DCAssociatedWindowSurfaceReference &other) noexcept {
    if(m_surface != other.m_surface) {
        if(m_surface)
            m_surface->release();

        m_surface = other.m_surface;

        if(m_surface)
            m_surface->addReference();
    }

    return *this;
}

DCAssociatedWindowSurfaceReference::DCAssociatedWindowSurfaceReference(DCAssociatedWindowSurfaceReference &&other) noexcept : DCAssociatedWindowSurfaceReference() {
    *this = std::move(other);
}

DCAssociatedWindowSurfaceReference &DCAssociatedWindowSurfaceReference::operator =(DCAssociatedWindowSurfaceReference &&other) noexcept {
    if(this != &other) {

        if(m_surface)
            m_surface->release();

        m_surface = other.m_surface;
        other.m_surface = nullptr;
    }

    return *this;
}

