#ifndef GLES_SDL_SDL_WRAPPER_H
#define GLES_SDL_SDL_WRAPPER_H

#include <GLES/GLESImplementationType.h>
#include <SDL2/SDL.h>

class SDLGLESImplementation;

void initializeSDLGLES(GLESImplementationType type);

SDL_Window *getSDLWindowForWebView();

extern SDLGLESImplementation *SelectedSDLGLESImplementation;;

#endif

