#include <GLES/SDLWrapper.h>

#include <GLES/GLESImplementationNative.h>

#include <GLES/ANGLE/GLESImplementationANGLE.h>

#include <SDL2/SDL_video.h>

#include <variant>
#include <stdexcept>

static std::variant<std::monostate, GLESImplementationNative, GLESImplementationANGLE> GLESImplementationStorage;
static GLESImplementation *SelectedGLESImplementation;

void initializeGLES(GLESImplementationType type) {
    switch(type) {
        case GLESImplementationType::Native:
            SelectedGLESImplementation = &GLESImplementationStorage.emplace<GLESImplementationNative>();
            break;

        case GLESImplementationType::ANGLE:
            SelectedGLESImplementation = &GLESImplementationStorage.emplace<GLESImplementationANGLE>();
            break;

        default:
            throw std::logic_error("initializeGLES: unsupported implementation type");
    }
}

SDL_Window * SDLCALL SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) {
    return SelectedGLESImplementation->CreateWindow(title, x, y, w, h, flags);
}

void SDLCALL SDL_DestroyWindow(SDL_Window *window) {
    SelectedGLESImplementation->DestroyWindow(window);
}

SDL_GLContext SDLCALL SDL_GL_CreateContext(SDL_Window *window) {
    return SelectedGLESImplementation->CreateContext(window);
}

void SDLCALL SDL_GL_DeleteContext(SDL_GLContext context) {
    SelectedGLESImplementation->DeleteContext(context);
}

SDL_GLContext SDLCALL SDL_GL_GetCurrentContext() {
    return SelectedGLESImplementation->GetCurrentContext();
}

SDL_Window *SDLCALL SDL_GL_GetCurrentWindow() {
    return SelectedGLESImplementation->GetCurrentWindow();
}


SDL_bool SDLCALL SDL_GL_ExtensionSupported(const char *extension) {
    return SelectedGLESImplementation->ExtensionSupported(extension);
}

int SDLCALL SDL_GL_GetAttribute(SDL_GLattr attr, int *value) {
    return SelectedGLESImplementation->GetAttribute(attr, value);
}

void *SDLCALL SDL_GL_GetProcAddress(const char *proc) {
    return SelectedGLESImplementation->GetProcAddress(proc);
}

int SDLCALL SDL_GL_GetSwapInterval() {
    return SelectedGLESImplementation->GetSwapInterval();
}

int SDLCALL SDL_GL_LoadLibrary(const char *path) {
    return SelectedGLESImplementation->LoadLibrary(path);
}

int SDLCALL SDL_GL_MakeCurrent(SDL_Window *window, SDL_GLContext context) {
    return SelectedGLESImplementation->MakeCurrent(window, context);
}

void SDLCALL SDL_GL_ResetAttributes(void) {
    SelectedGLESImplementation->ResetAttributes();
}

int SDLCALL SDL_GL_SetAttribute(SDL_GLattr attr, int value) {
    return SelectedGLESImplementation->SetAttribute(attr, value);
}

int SDLCALL SDL_GL_SetSwapInterval(int interval) {
    return SelectedGLESImplementation->SetSwapInterval(interval);
}


void SDLCALL SDL_GL_SwapWindow(SDL_Window *window) {
    return SelectedGLESImplementation->SwapWindow(window);
}

void SDLCALL SDL_GL_UnloadLibrary() {
    SelectedGLESImplementation->UnloadLibrary();
}
