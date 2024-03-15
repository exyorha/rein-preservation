#include <GLES/SDL/SDLGLESImplementationNative.h>
#include <GLES/SDL/RealSDLSymbols.h>

#include <SDL2/SDL_video.h>

SDLGLESImplementationNative::SDLGLESImplementationNative() = default;

SDLGLESImplementationNative::~SDLGLESImplementationNative() = default;

SDL_Window *SDLGLESImplementationNative::CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) noexcept {
    return RealSDLSymbols::getSingleton().realCreateWindow(title, x, y, w, h, flags);
}

void SDLGLESImplementationNative::DestroyWindowImpl(SDL_Window *window) noexcept {
    RealSDLSymbols::getSingleton().realDestroyWindow(window);
}

std::unique_ptr<BaseGLESContext> SDLGLESImplementationNative::CreateContextImpl(SDL_Window *window) {
    SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, SDL_TRUE);
#if 0
    SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    auto context = RealSDLSymbols::getSingleton().realGL_CreateContext(window);
    if(!context)
        return {};

    return std::make_unique<NativeWrapperContext>(context);
}

SDL_bool SDLGLESImplementationNative::ExtensionSupported(const char *extension) noexcept {
    return RealSDLSymbols::getSingleton().realGL_ExtensionSupported(extension);
}

int SDLGLESImplementationNative::MakeCurrentImpl(SDL_Window *window, SDL_GLContext context) noexcept {
    return RealSDLSymbols::getSingleton().realGL_MakeCurrent(window, NativeWrapperContext::unwrap(context));
}

int SDLGLESImplementationNative::GetAttribute(SDL_GLattr attr, int *value) noexcept {
    return RealSDLSymbols::getSingleton().realGL_GetAttribute(attr, value);
}

void SDLGLESImplementationNative::ResetAttributes() noexcept {
    return RealSDLSymbols::getSingleton().realGL_ResetAttributes();
}

int SDLGLESImplementationNative::SetAttribute(SDL_GLattr attr, int value) noexcept {
    return RealSDLSymbols::getSingleton().realGL_SetAttribute(attr, value);
}

int SDLGLESImplementationNative::GetSwapInterval() noexcept {
    return RealSDLSymbols::getSingleton().realGL_GetSwapInterval();
}

int SDLGLESImplementationNative::SetSwapInterval(int interval) noexcept {
    return RealSDLSymbols::getSingleton().realGL_SetSwapInterval(interval);
}

void SDLGLESImplementationNative::SwapWindow(SDL_Window *window) noexcept {
    return RealSDLSymbols::getSingleton().realGL_SwapWindow(window);
}

int SDLGLESImplementationNative::LoadLibrary(const char *path) noexcept {
    return RealSDLSymbols::getSingleton().realGL_LoadLibrary(path);
}

void SDLGLESImplementationNative::UnloadLibrary() noexcept {
    return RealSDLSymbols::getSingleton().realGL_UnloadLibrary();
}

SDLGLESImplementationNative::NativeWrapperContext::NativeWrapperContext(SDL_GLContext nativeContext) : m_context(nativeContext) {

}

SDLGLESImplementationNative::NativeWrapperContext::~NativeWrapperContext() {
    RealSDLSymbols::getSingleton().realGL_DeleteContext(m_context);
}

void *SDLGLESImplementationNative::NativeWrapperContext::getProcAddress(const char *name) noexcept {
    return RealSDLSymbols::getSingleton().realGL_GetProcAddress(name);
}
