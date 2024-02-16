#ifndef DC_ASSOCIATED_WINDOW_SURFACE_H
#define DC_ASSOCIATED_WINDOW_SURFACE_H

#include <windows.h>

#include <GLES/ANGLE/ANGLELibrary.h>

#include <memory>
#include <atomic>

class ANGLELibrary;
class DCAssociatedWindowSurfaceReference;
class EGLWindowSurface;

class DCAssociatedWindowSurface {
private:
    ~DCAssociatedWindowSurface();

public:
    DCAssociatedWindowSurface(const ANGLELibrary &angle, EGLDisplay display, HDC hDC);

    DCAssociatedWindowSurface(const DCAssociatedWindowSurface &other) = delete;
    DCAssociatedWindowSurface &operator =(const DCAssociatedWindowSurface &other) = delete;

    void addReference() noexcept;
    void release() noexcept;

    static DCAssociatedWindowSurfaceReference getSurfaceOfDC(HDC dc) noexcept;

    inline EGLConfig config() const {
        return m_config;
    }

    EGLSurface surface() const;

private:
    static void setSurfaceOfDC(HDC dc, DCAssociatedWindowSurface*surface);
    static DCAssociatedWindowSurface *getSurfaceOfWindow(HWND window) noexcept;
    static void setSurfaceOfWindow(HWND window, DCAssociatedWindowSurface *surface);

    class Association {
    public:
        Association(HDC hDC, DCAssociatedWindowSurface *surface);
        ~Association();

        Association(const Association &other) = delete;
        Association &operator =(const Association &other) = delete;

    private:
        HDC m_dc;
    };

    std::atomic_uint m_references;
    EGLConfig m_config;
    std::unique_ptr<EGLWindowSurface> m_surface;
    Association m_association;
};

class DCAssociatedWindowSurfaceReference {
public:
    explicit DCAssociatedWindowSurfaceReference(DCAssociatedWindowSurface *surface = nullptr) noexcept;
    ~DCAssociatedWindowSurfaceReference();

    DCAssociatedWindowSurfaceReference(const DCAssociatedWindowSurfaceReference &other) noexcept;
    DCAssociatedWindowSurfaceReference &operator =(const DCAssociatedWindowSurfaceReference &other) noexcept;

    DCAssociatedWindowSurfaceReference(DCAssociatedWindowSurfaceReference &&other) noexcept;
    DCAssociatedWindowSurfaceReference &operator =(DCAssociatedWindowSurfaceReference &&other) noexcept;

    inline DCAssociatedWindowSurface *get() const {
        return m_surface;
    }

    inline DCAssociatedWindowSurface &operator *() const {
        return *m_surface;
    }

    inline DCAssociatedWindowSurface *operator ->() const {
        return m_surface;
    }

    inline operator bool() const {
        return static_cast<bool>(m_surface);
    }

private:
    DCAssociatedWindowSurface *m_surface;
};

#endif
