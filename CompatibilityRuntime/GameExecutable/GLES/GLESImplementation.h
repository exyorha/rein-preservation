#ifndef GLES_GLES_IMPLEMENTATION_H
#define GLES_GLES_IMPLEMENTATION_H

#include <SDL2/SDL_video.h>

class GLESImplementation {
protected:
    GLESImplementation();
    ~GLESImplementation();

    GLESImplementation(const GLESImplementation &other) = delete;
    GLESImplementation &operator =(const GLESImplementation &other) = delete;

public:
    virtual SDL_Window *CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) noexcept = 0;
    virtual void DestroyWindow(SDL_Window *window) noexcept = 0;

    virtual SDL_GLContext CreateContext(SDL_Window *window) noexcept = 0;
    virtual void DeleteContext(SDL_GLContext context) noexcept = 0;

    virtual SDL_GLContext GetCurrentContext() noexcept = 0;
    virtual SDL_Window *GetCurrentWindow() noexcept = 0;
    virtual int MakeCurrent(SDL_Window *window, SDL_GLContext context) noexcept = 0;

    virtual void *GetProcAddress(const char *proc) noexcept = 0;
    virtual SDL_bool ExtensionSupported(const char *extension) noexcept = 0;

    virtual int GetAttribute(SDL_GLattr attr, int *value) noexcept = 0;
    virtual void ResetAttributes() noexcept = 0;
    virtual int SetAttribute(SDL_GLattr attr, int value) noexcept = 0;

    virtual int GetSwapInterval() noexcept = 0;
    virtual int SetSwapInterval(int interval) noexcept = 0;
    virtual void SwapWindow(SDL_Window *window) noexcept = 0;

    virtual int LoadLibrary(const char *path) noexcept = 0;
    virtual void UnloadLibrary() noexcept = 0;

};

#endif

