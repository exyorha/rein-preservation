#include <GLES/SDL/SDLWrapper.h>
#include <GLES/SDL/SDLGLESImplementationNative.h>
#include <GLES/SDL/SDLGLESImplementationANGLE.h>

#include <SDL2/SDL_video.h>

#include <variant>
#include <stdexcept>

static std::variant<std::monostate, SDLGLESImplementationNative, SDLGLESImplementationANGLE> SDLGLESImplementationStorage;
static SDLGLESImplementation *SelectedSDLGLESImplementation;

void initializeSDLGLES(GLESImplementationType type) {
    switch(type) {
        case GLESImplementationType::Native:
            SelectedSDLGLESImplementation = &SDLGLESImplementationStorage.emplace<SDLGLESImplementationNative>();
            break;

        case GLESImplementationType::ANGLE:
            SelectedSDLGLESImplementation = &SDLGLESImplementationStorage.emplace<SDLGLESImplementationANGLE>();
            break;

        default:
            throw std::logic_error("initializeGLES: unsupported implementation type");
    }
}

SDL_Window * SDLCALL SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) {
    return SelectedSDLGLESImplementation->CreateWindow(title, x, y, w, h, flags);
}

void SDLCALL SDL_DestroyWindow(SDL_Window *window) {
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
