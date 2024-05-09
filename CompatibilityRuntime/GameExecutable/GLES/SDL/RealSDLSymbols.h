#ifndef GLES_SDL_REAL_SDL_SYMBOLS_H
#define GLES_SDL_REAL_SDL_SYMBOLS_H

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_syswm.h>

#include <optional>

struct RealSDLSymbols {
public:
    RealSDLSymbols();
    ~RealSDLSymbols();

    RealSDLSymbols(const RealSDLSymbols &other) = delete;
    RealSDLSymbols &operator =(const RealSDLSymbols &other) = delete;

    using SDL_CreateWindow_Type = SDL_Window *(*SDLCALL)(const char *title, int x, int y, int w, int h, Uint32 flags);
    SDL_CreateWindow_Type realCreateWindow;

    using SDL_DestroyWindow_Type = void (*SDLCALL)(SDL_Window *window);
    SDL_DestroyWindow_Type realDestroyWindow;

    using SDL_GetWindowWMInfo_Type = SDL_bool (*SDLCALL)(SDL_Window * window, SDL_SysWMinfo * info);
    SDL_GetWindowWMInfo_Type realGetWindowWMInfo;

    using SDL_GL_GetProcAddress_Type = void *(*SDLCALL)(const char *proc);
    SDL_GL_GetProcAddress_Type realGL_GetProcAddress;

    using SDL_GL_GetDrawableSize_Type = void (*SDLCALL)(SDL_Window *window, int *w, int *h);
    SDL_GL_GetDrawableSize_Type realGL_GetDrawableSize;

    using SDL_GL_SwapWindow_Type = void (*SDLCALL)(SDL_Window *window);
    SDL_GL_SwapWindow_Type realGL_SwapWindow;

    static const RealSDLSymbols &getSingleton();

private:
    [[noreturn]] static void failedToBind(const char *symbol);

    static std::optional<RealSDLSymbols> m_singleton;
};

#endif
