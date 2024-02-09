#ifndef GLES_ANGLE_SDL_WRAPPER_ATTRIBUTE_SET_H
#define GLES_ANGLE_SDL_WRAPPER_ATTRIBUTE_SET_H

#include <GLES/ANGLE/ANGLELibrary.h>

#include <array>

class SDLWrapperAttributeSet {
public:
    SDLWrapperAttributeSet();
    ~SDLWrapperAttributeSet();

    SDLWrapperAttributeSet(const SDLWrapperAttributeSet &other);
    SDLWrapperAttributeSet &operator =(const SDLWrapperAttributeSet &other);

    std::array<int, 28> attributes;

    EGLConfig chooseConfig(const ANGLELibrary &angle, EGLDisplay display) const;
};

#endif
