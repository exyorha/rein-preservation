#ifndef GLES_SDL_SDL_WRAPPER_H
#define GLES_SDL_SDL_WRAPPER_H

#include <SDL2/SDL.h>

class SDLGLESImplementation;

void initializeSDLGLES();

SDL_Window *getSDLWindowForWebView();

extern SDLGLESImplementation *SelectedSDLGLESImplementation;;

#endif

