#include <GLES/ANGLE/SDLWrapperAttributeSet.h>

#include <SDL2/SDL_video.h>

#include <stdexcept>

SDLWrapperAttributeSet::SDLWrapperAttributeSet() {
    for(auto &attrib: attributes) {
        attrib = 0;
    }

    attributes[SDL_GL_RED_SIZE] = 3;
    attributes[SDL_GL_GREEN_SIZE] = 3;
    attributes[SDL_GL_BLUE_SIZE] = 2;
    attributes[SDL_GL_DOUBLEBUFFER] = SDL_TRUE;
    attributes[SDL_GL_DEPTH_SIZE] = 16;
    attributes[SDL_GL_ACCELERATED_VISUAL] = -1;
    attributes[SDL_GL_CONTEXT_RELEASE_BEHAVIOR] = 1;
}

SDLWrapperAttributeSet::~SDLWrapperAttributeSet() = default;

SDLWrapperAttributeSet::SDLWrapperAttributeSet(const SDLWrapperAttributeSet &other) = default;

SDLWrapperAttributeSet &SDLWrapperAttributeSet::operator =(const SDLWrapperAttributeSet &other) = default;

EGLConfig SDLWrapperAttributeSet::chooseConfig(const ANGLELibrary &angle, EGLDisplay display) const {

    int configAttributes[]{
        EGL_RED_SIZE, attributes[SDL_GL_RED_SIZE],
        EGL_GREEN_SIZE, attributes[SDL_GL_GREEN_SIZE],
        EGL_BLUE_SIZE, attributes[SDL_GL_BLUE_SIZE],
        EGL_ALPHA_SIZE, attributes[SDL_GL_ALPHA_SIZE],
        EGL_BUFFER_SIZE, attributes[SDL_GL_BUFFER_SIZE],
        // SDL_GL_DOUBLEBUFFER: not mappable?
        EGL_DEPTH_SIZE, attributes[SDL_GL_DEPTH_SIZE],
        EGL_STENCIL_SIZE, attributes[SDL_GL_STENCIL_SIZE],
        // SDL_GL_ACCUM_* - not mappable and shouldn't be ever used
        EGL_SAMPLE_BUFFERS, attributes[SDL_GL_MULTISAMPLEBUFFERS],
        EGL_SAMPLES, attributes[SDL_GL_MULTISAMPLESAMPLES],
        EGL_NONE
    };

    EGLConfig config;
    int availableConfigs;
    if(!angle.eglChooseConfig(display, configAttributes, &config, 1, &availableConfigs) || availableConfigs == 0)
        throw std::runtime_error("EGL_ChooseConfig has failed");

    return config;
}
