#include <GLES/SDL/SDLWrapper.h>
#include <GLES/SDL/RealSDLSymbols.h>

#include <SDL2/SDL_video.h>

#include <algorithm>
#include <vector>

static std::vector<SDL_Window *> ExistingSDLWindows;

static void registerSDLWindow(SDL_Window *window) {
    ExistingSDLWindows.emplace_back(window);
}

static void unregisterSDLWindow(SDL_Window *window) {
    auto it = std::find(ExistingSDLWindows.begin(), ExistingSDLWindows.end(), window);
    if(it != ExistingSDLWindows.end())
        ExistingSDLWindows.erase(it);
}

SDL_Window *getSDLWindowForWebView() {
    if(ExistingSDLWindows.empty())
        return nullptr;

    return ExistingSDLWindows.front();
}

SDL_Window * SDLCALL SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) {
    auto window = RealSDLSymbols::getSingleton().realCreateWindow(title, x, y, w, h, flags);
    if(window) {
        registerSDLWindow(window);
    }
    return window;
}

void SDLCALL SDL_DestroyWindow(SDL_Window *window) {
    unregisterSDLWindow(window);

    RealSDLSymbols::getSingleton().realDestroyWindow(window);
}
