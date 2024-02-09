#ifndef GLES_GLES_IMPLEMENTATION_NATIVE_H
#define GLES_GLES_IMPLEMENTATION_NATIVE_H

#include <GLES/GLESImplementation.h>

class GLESImplementationNative final : public GLESImplementation {
public:
    GLESImplementationNative();
    ~GLESImplementationNative();

    SDL_Window *CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) noexcept override;
    void DestroyWindow(SDL_Window *window) noexcept override;

    SDL_GLContext CreateContext(SDL_Window *window) noexcept override;
    void DeleteContext(SDL_GLContext context) noexcept override;

    void *GetProcAddress(const char *proc) noexcept override;
    SDL_bool ExtensionSupported(const char *extension) noexcept override;

    SDL_Window *GetCurrentWindow() noexcept override;
    SDL_GLContext GetCurrentContext() noexcept override;
    int MakeCurrent(SDL_Window *window, SDL_GLContext context) noexcept override;

    int GetAttribute(SDL_GLattr attr, int *value) noexcept override;
    void ResetAttributes() noexcept override;
    int SetAttribute(SDL_GLattr attr, int value) noexcept override;

    int GetSwapInterval() noexcept override;
    int SetSwapInterval(int interval) noexcept override;
    void SwapWindow(SDL_Window *window) noexcept override;

    int LoadLibrary(const char *path) noexcept override;
    void UnloadLibrary() noexcept override;

};

#endif
