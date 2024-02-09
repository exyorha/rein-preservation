#ifndef GLES_SDL_WRAPPER_H
#define GLES_SDL_WRAPPER_H

enum class GLESImplementationType {
    Native,
    ANGLE
};

void initializeGLES(GLESImplementationType type);

#endif

