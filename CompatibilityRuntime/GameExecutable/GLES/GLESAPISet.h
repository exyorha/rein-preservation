#ifndef GLES_GLES_API_SET_H
#define GLES_GLES_API_SET_H

#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>

class GLESAPISet {
public:
    GLESAPISet();
    ~GLESAPISet();

    GLESAPISet(const GLESAPISet &other) = delete;
    GLESAPISet &operator =(const GLESAPISet &other) = delete;

    PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
    PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
    PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
    PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;

    PFNGLGENTEXTURESPROC glGenTextures;
    PFNGLDELETETEXTURESPROC glDeleteTextures;
    PFNGLACTIVETEXTUREPROC glActiveTexture;
    PFNGLBINDTEXTUREPROC glBindTexture;
    PFNGLTEXIMAGE2DPROC glTexImage2D;
    PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;
    PFNGLTEXPARAMETERIPROC glTexParameteri;

    PFNGLLABELOBJECTEXTPROC glLabelObjectEXT;

    PFNGLDRAWARRAYSPROC glDrawArrays;

    PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
    PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
    PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

    PFNGLENABLEPROC glEnable;
    PFNGLDISABLEPROC glDisable;
    PFNGLVIEWPORTPROC glViewport;
    PFNGLGETINTEGERVPROC glGetIntegerv;
    PFNGLGETBOOLEANVPROC glGetBooleanv;

    PFNGLCREATEPROGRAMPROC glCreateProgram;
    PFNGLDELETEPROGRAMPROC glDeleteProgram;
    PFNGLUSEPROGRAMPROC glUseProgram;
    PFNGLATTACHSHADERPROC glAttachShader;
    PFNGLDETACHSHADERPROC glDetachShader;
    PFNGLLINKPROGRAMPROC glLinkProgram;
    PFNGLGETPROGRAMIVPROC glGetProgramiv;
    PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

    PFNGLCREATESHADERPROC glCreateShader;
    PFNGLDELETESHADERPROC glDeleteShader;
    PFNGLSHADERSOURCEPROC glShaderSource;
    PFNGLCOMPILESHADERPROC glCompileShader;
    PFNGLGETSHADERIVPROC glGetShaderiv;
    PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;

    static const GLESAPISet &get();

    static void *getGLProcAddress(const char *name);

    void setEnabled(GLenum token, GLboolean value) const;

private:
    template<typename Result, typename... Args>
    static inline void bindFunction(const char *name, Result (*&pointer)(Args... args),
                                    bool optional = false) {
        pointer = reinterpret_cast<Result (*)(Args...)>(bindFunction(name, optional));
    }

    static void *bindFunction(const char *name, bool optional = false);

};

#endif

