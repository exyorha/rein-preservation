#include <GLES/GLESRenderingOverlay.h>

std::atomic<GLESRenderingOverlay *> GLESRenderingOverlay::m_overlay(nullptr);

GLESRenderingOverlay::GLESRenderingOverlay() = default;

GLESRenderingOverlay::~GLESRenderingOverlay() = default;

void GLESRenderingOverlay::beforeSwapBuffers(int32_t drawableWidth, int32_t drawableHeight) {
    (void)drawableWidth;
    (void)drawableHeight;
}

void GLESRenderingOverlay::afterSwapBuffers() {

}

void GLESRenderingOverlay::install(GLESRenderingOverlay *overlay) noexcept {
    m_overlay.store(overlay);
}

void GLESRenderingOverlay::invokeBeforeSwapBuffers(int32_t drawableWidth, int32_t drawableHeight) {
    auto overlay = m_overlay.load();
    if(overlay) {
        overlay->beforeSwapBuffers(drawableWidth, drawableHeight);
    }
}

void GLESRenderingOverlay::invokeAfterSwapBuffers() {
    auto overlay = m_overlay.load();
    if(overlay) {
        overlay->afterSwapBuffers();
    }
}
