#ifndef GLES_SDL_SDL_WRAPPER_H
#define GLES_SDL_SDL_WRAPPER_H

#include <GLES/GLESImplementationType.h>
#include <SDL2/SDL.h>

void initializeSDLGLES(GLESImplementationType type);

SDL_Window *getSDLWindowForWebView();

#endif

