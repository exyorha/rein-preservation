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

SDL_GLContext GLESImplementationNative::CreateContext(SDL_Window *window) noexcept {
    SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, SDL_TRUE);
    return RealSDLSymbols::getSingleton().realGL_CreateContext(window);
}

void GLESImplementationNative::DeleteContextImpl(SDL_GLContext context) noexcept {
    return RealSDLSymbols::getSingleton().realGL_DeleteContext(context);
}

void *GLESImplementationNative::GetProcAddress(const char *proc) noexcept {
    return RealSDLSymbols::getSingleton().realGL_GetProcAddress(proc);
}

SDL_bool GLESImplementationNative::ExtensionSupported(const char *extension) noexcept {
    return RealSDLSymbols::getSingleton().realGL_ExtensionSupported(extension);
}

int GLESImplementationNative::MakeCurrentImpl(SDL_Window *window, SDL_GLContext context) noexcept {
    return RealSDLSymbols::getSingleton().realGL_MakeCurrent(window, context);
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
