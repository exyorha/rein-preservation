#include "GLES/BaseGLESContext.h"
#include <GLES/GLESImplementationNative.h>
#include <GLES/RealSDLSymbols.h>
#include <SDL2/SDL_video.h>

GLESImplementationNative::GLESImplementationNative() = default;

GLESImplementationNative::~GLESImplementationNative() = default;

SDL_Window *GLESImplementationNative::CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) noexcept {
    return RealSDLSymbols::getSingleton().realCreateWindow(title, x, y, w, h, flags);
}

void GLESImplementationNative::DestroyWindowImpl(SDL_Window *window) noexcept {
    RealSDLSymbols::getSingleton().realDestroyWindow(window);
}

std::unique_ptr<BaseGLESContext> GLESImplementationNative::CreateContextImpl(SDL_Window *window) {
    SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, SDL_TRUE);

    auto context = RealSDLSymbols::getSingleton().realGL_CreateContext(window);
    if(!context)
        return {};

    return std::make_unique<NativeWrapperContext>(context);
}

SDL_bool GLESImplementationNative::ExtensionSupported(const char *extension) noexcept {
    return RealSDLSymbols::getSingleton().realGL_ExtensionSupported(extension);
}

int GLESImplementationNative::MakeCurrentImpl(SDL_Window *window, SDL_GLContext context) noexcept {
    return RealSDLSymbols::getSingleton().realGL_MakeCurrent(window, NativeWrapperContext::unwrap(context));
}

int GLESImplementationNative::GetAttribute(SDL_GLattr attr, int *value) noexcept {
    return RealSDLSymbols::getSingleton().realGL_GetAttribute(attr, value);
}

void GLESImplementationNative::ResetAttributes() noexcept {
    return RealSDLSymbols::getSingleton().realGL_ResetAttributes();
}

int GLESImplementationNative::SetAttribute(SDL_GLattr attr, int value) noexcept {
    return RealSDLSymbols::getSingleton().realGL_SetAttribute(attr, value);
}

int GLESImplementationNative::GetSwapInterval() noexcept {
    return RealSDLSymbols::getSingleton().realGL_GetSwapInterval();
}

int GLESImplementationNative::SetSwapInterval(int interval) noexcept {
    return RealSDLSymbols::getSingleton().realGL_SetSwapInterval(interval);
}

void GLESImplementationNative::SwapWindow(SDL_Window *window) noexcept {
    return RealSDLSymbols::getSingleton().realGL_SwapWindow(window);
}

int GLESImplementationNative::LoadLibrary(const char *path) noexcept {
    return RealSDLSymbols::getSingleton().realGL_LoadLibrary(path);
}

void GLESImplementationNative::UnloadLibrary() noexcept {
    return RealSDLSymbols::getSingleton().realGL_UnloadLibrary();
}

GLESImplementationNative::NativeWrapperContext::NativeWrapperContext(SDL_GLContext nativeContext) : m_context(nativeContext) {

}

GLESImplementationNative::NativeWrapperContext::~NativeWrapperContext() {
    RealSDLSymbols::getSingleton().realGL_DeleteContext(m_context);
}

void *GLESImplementationNative::NativeWrapperContext::getProcAddress(const char *name) noexcept {
    return RealSDLSymbols::getSingleton().realGL_GetProcAddress(name);
}
