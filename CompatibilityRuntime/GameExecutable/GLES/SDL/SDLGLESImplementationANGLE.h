#ifndef GLES_SDL_SDL_GLES_IMPLEMENTATION_ANGLE_H
#define GLES_SDL_SDL_GLES_IMPLEMENTATION_ANGLE_H

#include <GLES/SDL/SDLGLESImplementation.h>

#include <GLES/ANGLE/ANGLELibrary.h>
#include <GLES/SDL/SDLWrapperAttributeSet.h>
#include <GLES/ANGLE/InitializedEGLDisplay.h>

#include <optional>

#include <SDL2/SDL_syswm.h>

class InitializedEGLContext;
class EGLWindowSurface;

class SDLGLESImplementationANGLE final : public SDLGLESImplementation {
public:
    SDLGLESImplementationANGLE();
    ~SDLGLESImplementationANGLE();

    SDL_Window *CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) noexcept override;

protected:
    void DestroyWindowImpl(SDL_Window *window) noexcept override;

    std::unique_ptr<BaseGLESContext> CreateContextImpl(SDL_Window *window) override;

    int MakeCurrentImpl(SDL_Window *window, SDL_GLContext context) noexcept override;

public:
    SDL_bool ExtensionSupported(const char *extension) noexcept override;

    int GetAttribute(SDL_GLattr attr, int *value) noexcept override;
    void ResetAttributes() noexcept override;
    int SetAttribute(SDL_GLattr attr, int value) noexcept override;

    int GetSwapInterval() noexcept override;
    int SetSwapInterval(int interval) noexcept override;
    void SwapWindow(SDL_Window *window) noexcept override;

    int LoadLibrary(const char *path) noexcept override;
    void UnloadLibrary() noexcept override;

private:

    static EGLWindowSurface *getSurfaceOfWindow(SDL_Window *window);
    static void setSurfaceOfWindow(SDL_Window *window, EGLWindowSurface *surface);

    static bool isCompatibleWith(const SDL_SysWMinfo &first, const SDL_SysWMinfo &other);

    ANGLELibrary m_angle;
    SDLWrapperAttributeSet m_currentAttributes;
    std::optional<InitializedEGLDisplay> m_display;
    std::optional<SDL_SysWMinfo> m_displayWM;

};

#endif

