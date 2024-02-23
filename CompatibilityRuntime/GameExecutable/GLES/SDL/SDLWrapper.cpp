#include <GLES/SDL/SDLWrapper.h>
#include <GLES/SDL/SDLGLESImplementationNative.h>
#include <GLES/SDL/SDLGLESImplementationANGLE.h>
#include <GLES/Shim/GLESContextShim.h>

#include <SDL2/SDL_video.h>

#include <variant>
#include <stdexcept>
#include <algorithm>

SDLGLESImplementation *SelectedSDLGLESImplementation;
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

void initializeSDLGLES(GLESImplementationType type) {
    switch(type) {
        case GLESImplementationType::Native:
            SelectedSDLGLESImplementation = new SDLGLESImplementationNative;
            break;

        case GLESImplementationType::ANGLE:
            SelectedSDLGLESImplementation = new SDLGLESImplementationANGLE;
            break;

        default:
            throw std::logic_error("initializeGLES: unsupported implementation type");
    }
}

SDL_Window * SDLCALL SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) {
    auto window = SelectedSDLGLESImplementation->CreateWindow(title, x, y, w, h, flags);
    if(window) {
        registerSDLWindow(window);
    }
    return window;
}

void SDLCALL SDL_DestroyWindow(SDL_Window *window) {
    unregisterSDLWindow(window);

    SelectedSDLGLESImplementation->DestroyWindow(window);
}

SDL_GLContext SDLCALL SDL_GL_CreateContext(SDL_Window *window) {
    return SelectedSDLGLESImplementation->CreateContext(window);
}

void SDLCALL SDL_GL_DeleteContext(SDL_GLContext context) {
    SelectedSDLGLESImplementation->DeleteContext(context);
}

SDL_GLContext SDLCALL SDL_GL_GetCurrentContext() {
    return SelectedSDLGLESImplementation->GetCurrentContext();
}

SDL_Window *SDLCALL SDL_GL_GetCurrentWindow() {
    return SelectedSDLGLESImplementation->GetCurrentWindow();
}


SDL_bool SDLCALL SDL_GL_ExtensionSupported(const char *extension) {
    return SelectedSDLGLESImplementation->ExtensionSupported(extension);
}

int SDLCALL SDL_GL_GetAttribute(SDL_GLattr attr, int *value) {
    return SelectedSDLGLESImplementation->GetAttribute(attr, value);
}

void *SDLCALL SDL_GL_GetProcAddress(const char *proc) {
    return SelectedSDLGLESImplementation->GetProcAddress(proc);
}

int SDLCALL SDL_GL_GetSwapInterval() {
    return SelectedSDLGLESImplementation->GetSwapInterval();
}

int SDLCALL SDL_GL_LoadLibrary(const char *path) {
    return SelectedSDLGLESImplementation->LoadLibrary(path);
}

int SDLCALL SDL_GL_MakeCurrent(SDL_Window *window, SDL_GLContext context) {
    return SelectedSDLGLESImplementation->MakeCurrent(window, context);
}

void SDLCALL SDL_GL_ResetAttributes(void) {
    SelectedSDLGLESImplementation->ResetAttributes();
}

int SDLCALL SDL_GL_SetAttribute(SDL_GLattr attr, int value) {
    return SelectedSDLGLESImplementation->SetAttribute(attr, value);
}

int SDLCALL SDL_GL_SetSwapInterval(int interval) {
    return SelectedSDLGLESImplementation->SetSwapInterval(interval);
}


void SDLCALL SDL_GL_SwapWindow(SDL_Window *window) {
    return SelectedSDLGLESImplementation->SwapWindow(window);
}

void SDLCALL SDL_GL_UnloadLibrary() {
    SelectedSDLGLESImplementation->UnloadLibrary();
}

GLESContextShim *GLESContextShim::getCurrentShim() {
    return static_cast<GLESContextShim *>(SDL_GL_GetCurrentContext());
}
