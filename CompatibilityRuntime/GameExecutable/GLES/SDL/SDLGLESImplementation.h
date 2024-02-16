#ifndef GLES_SDL_SDL_GLES_IMPLEMENTATION_H
#define GLES_SDL_SDL_GLES_IMPLEMENTATION_H

#include <SDL2/SDL_video.h>

#include <memory>

class BaseGLESContext;

class SDLGLESImplementation {
protected:
    SDLGLESImplementation();
    ~SDLGLESImplementation();

    SDLGLESImplementation(const SDLGLESImplementation &other) = delete;
    SDLGLESImplementation &operator =(const SDLGLESImplementation &other) = delete;

public:
    virtual SDL_Window *CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) noexcept = 0;
    void DestroyWindow(SDL_Window *window) noexcept;

protected:
    virtual void DestroyWindowImpl(SDL_Window *window) noexcept = 0;

public:
    SDL_GLContext CreateContext(SDL_Window *window) noexcept;
    void DeleteContext(SDL_GLContext context) noexcept;

protected:
    virtual std::unique_ptr<BaseGLESContext> CreateContextImpl(SDL_Window *window) = 0;

public:

    SDL_GLContext GetCurrentContext() noexcept;
    SDL_Window *GetCurrentWindow() noexcept;
    int MakeCurrent(SDL_Window *window, SDL_GLContext context) noexcept;

protected:
    virtual int MakeCurrentImpl(SDL_Window *window, SDL_GLContext context) noexcept = 0;

    BaseGLESContext *currentImplementationContext() const noexcept;

public:
    void *GetProcAddress(const char *proc) noexcept;
    virtual SDL_bool ExtensionSupported(const char *extension) noexcept = 0;

    virtual int GetAttribute(SDL_GLattr attr, int *value) noexcept = 0;
    virtual void ResetAttributes() noexcept = 0;
    virtual int SetAttribute(SDL_GLattr attr, int value) noexcept = 0;

    virtual int GetSwapInterval() noexcept = 0;
    virtual int SetSwapInterval(int interval) noexcept = 0;
    virtual void SwapWindow(SDL_Window *window) noexcept = 0;

    virtual int LoadLibrary(const char *path) noexcept = 0;
    virtual void UnloadLibrary() noexcept = 0;

private:

    static thread_local SDL_Window *m_currentWindow;
    static thread_local SDL_GLContext m_currentContext;

};

#endif

