#include <GLES/GLESRenderingOverlay.h>

std::atomic<GLESRenderingOverlay *> GLESRenderingOverlay::m_overlay(nullptr);

GLESRenderingOverlay::GLESRenderingOverlay() = default;

GLESRenderingOverlay::~GLESRenderingOverlay() = default;

void GLESRenderingOverlay::beforeSwapBuffers() {

}

void GLESRenderingOverlay::afterSwapBuffers() {

}

void GLESRenderingOverlay::install(GLESRenderingOverlay *overlay) noexcept {
    m_overlay.store(overlay);
}

void GLESRenderingOverlay::invokeBeforeSwapBuffers() {
    auto overlay = m_overlay.load();
    if(overlay) {
        overlay->beforeSwapBuffers();
    }
}

void GLESRenderingOverlay::invokeAfterSwapBuffers() {
    auto overlay = m_overlay.load();
    if(overlay) {
        overlay->afterSwapBuffers();
    }
}
