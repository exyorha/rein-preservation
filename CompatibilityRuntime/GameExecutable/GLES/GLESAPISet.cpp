#include <GLES/GLESAPISet.h>

#include <sstream>
#include <stdexcept>
#include <optional>

#ifdef _WIN32
#include <windows.h>
#else
#include <GLES/SDL/SDLWrapper.h>
#include <GLES/SDL/RealSDLSymbols.h>
#endif


GLESAPISet::GLESAPISet() {
    bindFunction("glGenFramebuffers", glGenFramebuffers);
    bindFunction("glDeleteFramebuffers", glDeleteFramebuffers);
    bindFunction("glBindFramebuffer", glBindFramebuffer);
    bindFunction("glFramebufferTexture2D", glFramebufferTexture2D);

    bindFunction("glGenTextures", glGenTextures);
    bindFunction("glDeleteTextures", glDeleteTextures);
    bindFunction("glActiveTexture", glActiveTexture);
    bindFunction("glBindTexture", glBindTexture);
    bindFunction("glTexImage2D", glTexImage2D);
    bindFunction("glTexSubImage2D", glTexSubImage2D);
    bindFunction("glTexParameteri", glTexParameteri);

    bindFunction("glLabelObjectEXT", glLabelObjectEXT, true);

    bindFunction("glDrawArrays", glDrawArrays);
    bindFunction("glGenVertexArrays", glGenVertexArrays);
    bindFunction("glDeleteVertexArrays", glDeleteVertexArrays);
    bindFunction("glBindVertexArray", glBindVertexArray);

    bindFunction("glEnable", glEnable);
    bindFunction("glDisable", glDisable);
    bindFunction("glViewport", glViewport);
    bindFunction("glGetIntegerv", glGetIntegerv);
    bindFunction("glGetBooleanv", glGetBooleanv);

    bindFunction("glCreateProgram", glCreateProgram);
    bindFunction("glDeleteProgram", glDeleteProgram);
    bindFunction("glUseProgram", glUseProgram);
    bindFunction("glAttachShader", glAttachShader);
    bindFunction("glDetachShader", glDetachShader);
    bindFunction("glLinkProgram", glLinkProgram);
    bindFunction("glGetProgramiv", glGetProgramiv);
    bindFunction("glGetProgramInfoLog", glGetProgramInfoLog);

    bindFunction("glCreateShader", glCreateShader);
    bindFunction("glDeleteShader", glDeleteShader);
    bindFunction("glShaderSource", glShaderSource);
    bindFunction("glCompileShader", glCompileShader);
    bindFunction("glGetShaderiv", glGetShaderiv);
    bindFunction("glGetShaderInfoLog", glGetShaderInfoLog);
}

GLESAPISet::~GLESAPISet() = default;

void *GLESAPISet::bindFunction(const char *name, bool optional) {
    auto result = getGLProcAddress(name);
    if(!result && !optional) {
        std::stringstream stream;
        stream << "failed to retrieve the required OpenGL API function " << name;
        throw std::runtime_error(stream.str());
    }

    return result;
}

void *GLESAPISet::getGLProcAddress(const char *name) {
#ifdef _WIN32

    static HMODULE opengl32Module = GetModuleHandle(L"opengl32.dll");

    auto proc = wglGetProcAddress(name);
    if(proc) {
        return reinterpret_cast<void *>(proc);
    }

    return reinterpret_cast<void *>(GetProcAddress(opengl32Module, name));
#else
    return RealSDLSymbols::getSingleton().realGL_GetProcAddress(name);
#endif
}

const GLESAPISet &GLESAPISet::get() {
    static std::optional<GLESAPISet> apiSet;
    if(!apiSet.has_value()) {
        apiSet.emplace();
    }

    return *apiSet;
}

void GLESAPISet::setEnabled(GLenum token, GLboolean value) const {
    if(value == GL_FALSE)
        glDisable(token);
    else
        glEnable(token);
}
