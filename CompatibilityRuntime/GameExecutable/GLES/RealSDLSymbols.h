#ifndef GLES_REAL_SDL_SYMBOLS_H
#define GLES_REAL_SDL_SYMBOLS_H

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

    using SDL_GL_CreateContext_Type = SDL_GLContext (*SDLCALL)(SDL_Window *window);
    SDL_GL_CreateContext_Type realGL_CreateContext;

    using SDL_GL_DeleteContext_Type = void (*SDLCALL)(SDL_GLContext context);
    SDL_GL_DeleteContext_Type realGL_DeleteContext;

    using SDL_GL_ExtensionSupported_Type = SDL_bool (*SDLCALL)(const char *extension);
    SDL_GL_ExtensionSupported_Type realGL_ExtensionSupported;

    using SDL_GL_GetAttribute_Type = int (*SDLCALL)(SDL_GLattr attr, int *value);
    SDL_GL_GetAttribute_Type realGL_GetAttribute;

    //using SDL_GL_GetCurrentContext_Type = SDL_GLContext (*SDLCALL)(void);
    //SDL_GL_GetCurrentContext_Type realGL_GetCurrentContext;

    //using SDL_GL_GetCurrentWindow_Type = SDL_Window *(*SDLCALL)(void);
    //SDL_GL_GetCurrentWindow_Type realGL_GetCurrentWindow;

    // SDL_GL_GetDrawableSize - we don't intercept because the original
    // function does exactly what we want

    using SDL_GL_GetProcAddress_Type = void *(*SDLCALL)(const char *proc);
    SDL_GL_GetProcAddress_Type realGL_GetProcAddress;

    using SDL_GL_GetSwapInterval_Type = int (*SDLCALL)(void);
    SDL_GL_GetSwapInterval_Type realGL_GetSwapInterval;

    using SDL_GL_LoadLibrary_Type = int (*SDLCALL)(const char *path);
    SDL_GL_LoadLibrary_Type realGL_LoadLibrary;

    using SDL_GL_MakeCurrent_Type = int (*SDLCALL)(SDL_Window *window, SDL_GLContext context);
    SDL_GL_MakeCurrent_Type realGL_MakeCurrent;

    using SDL_GL_ResetAttributes_Type = void (*SDLCALL)(void);
    SDL_GL_ResetAttributes_Type realGL_ResetAttributes;

    using SDL_GL_SetAttribute_Type = int (*SDLCALL)(SDL_GLattr attr, int value);
    SDL_GL_SetAttribute_Type realGL_SetAttribute;

    using SDL_GL_SetSwapInterval_Type = int (*SDLCALL)(int interval);
    SDL_GL_SetSwapInterval_Type realGL_SetSwapInterval;

    using SDL_GL_SwapWindow_Type = void (*SDLCALL)(SDL_Window *window);
    SDL_GL_SwapWindow_Type realGL_SwapWindow;

    // SDL_GL_UnbindTexture - is this one used?

    using SDL_GL_UnloadLibrary_Type = void (*SDLCALL)(void);
    SDL_GL_UnloadLibrary_Type realGL_UnloadLibrary;

    static const RealSDLSymbols &getSingleton();

private:
    [[noreturn]] static void failedToBind(const char *symbol);

    static std::optional<RealSDLSymbols> m_singleton;
};

#endif
