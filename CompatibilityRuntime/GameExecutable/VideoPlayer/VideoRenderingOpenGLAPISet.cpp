#include <VideoPlayer/VideoRenderingOpenGLAPISet.h>
#include <VideoPlayer/MPVTypes.h>

#include <sstream>
#include <stdexcept>

VideoRenderingOpenGLAPISet::VideoRenderingOpenGLAPISet(const mpv_opengl_init_params *gl) {
    bindFunction(gl, "glGenFramebuffers", glGenFramebuffers);
    bindFunction(gl, "glDeleteFramebuffers", glDeleteFramebuffers);
    bindFunction(gl, "glBindFramebuffer", glBindFramebuffer);
    bindFunction(gl, "glFramebufferTexture2D", glFramebufferTexture2D);

    bindFunction(gl, "glGenTextures", glGenTextures);
    bindFunction(gl, "glDeleteTextures", glDeleteTextures);
    bindFunction(gl, "glActiveTexture", glActiveTexture);
    bindFunction(gl, "glBindTexture", glBindTexture);
    bindFunction(gl, "glTexImage2D", glTexImage2D);
    bindFunction(gl, "glTexParameteri", glTexParameteri);

    bindFunction(gl, "glLabelObjectEXT", glLabelObjectEXT, true);
}

VideoRenderingOpenGLAPISet::~VideoRenderingOpenGLAPISet() = default;

void *VideoRenderingOpenGLAPISet::bindFunction(const mpv_opengl_init_params *openglImplementationInfo, const char *name, bool optional) {
    auto result = openglImplementationInfo->get_proc_address(openglImplementationInfo->get_proc_address_ctx, name);
    if(!result && !optional) {
        std::stringstream stream;
        stream << "failed to retrieve the required OpenGL API function " << name;
        throw std::runtime_error(stream.str());
    }

    return result;
}
