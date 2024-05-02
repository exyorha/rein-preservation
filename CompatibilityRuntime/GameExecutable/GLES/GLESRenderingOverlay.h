#ifndef GLES_GLES_RENDERING_OVERLAY_H
#define GLES_GLES_RENDERING_OVERLAY_H

#include <atomic>

class GLESRenderingOverlay {
protected:
    GLESRenderingOverlay();
    ~GLESRenderingOverlay();

public:
    GLESRenderingOverlay(const GLESRenderingOverlay &other) = delete;
    GLESRenderingOverlay &operator =(const GLESRenderingOverlay &other) = delete;

    virtual void beforeSwapBuffers(int32_t drawableWidth, int32_t drawableHeight);
    virtual void afterSwapBuffers();

    static void install(GLESRenderingOverlay *overlay) noexcept;

    static void invokeBeforeSwapBuffers(int32_t drawableWidth, int32_t drawableHeight);
    static void invokeAfterSwapBuffers();

    class Installer {
    public:
        explicit inline Installer(GLESRenderingOverlay *overlay) noexcept {
            GLESRenderingOverlay::install(overlay);
        }

        inline ~Installer() {
            GLESRenderingOverlay::install(nullptr);
        }

        Installer(const Installer &other) = delete;
        Installer &operator =(const Installer &other) = delete;
    };

private:
    static std::atomic<GLESRenderingOverlay *> m_overlay;
};

#endif
