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

    // SDL_GL_BindTexture - is this one used?

    using SDL_CreateWindow_Type = SDL_Window *(*SDLCALL)(const char *title, int x, int y, int w, int h, Uint32 flags);
    SDL_CreateWindow_Type realCreateWindow;

    using SDL_DestroyWindow_Type = void (*SDLCALL)(SDL_Window *window);
    SDL_DestroyWindow_Type realDestroyWindow;

    using SDL_GetWindowData_Type = void *(*SDLCALL)(SDL_Window *window, const char *name);
    SDL_GetWindowData_Type realGetWindowData;

    using SDL_SetWindowData_Type = void *(*SDLCALL)(SDL_Window *window, const char *name, void *userdata);
    SDL_SetWindowData_Type realSetWindowData;

    using SDL_GetWindowWMInfo_Type = SDL_bool (*SDLCALL)(SDL_Window * window, SDL_SysWMinfo * info);
    SDL_GetWindowWMInfo_Type realGetWindowWMInfo;

    using SDL_GL_GetProcAddress_Type = void *(*SDLCALL)(const char *proc);
    SDL_GL_GetProcAddress_Type realGL_GetProcAddress;

    using SDL_GetWindowSize_Type = void (*SDLCALL)(SDL_Window *window, int *w, int *h);
    SDL_GetWindowSize_Type realGetWindowSize;

    static const RealSDLSymbols &getSingleton();

private:
    [[noreturn]] static void failedToBind(const char *symbol);

    static std::optional<RealSDLSymbols> m_singleton;
};

#endif
