#ifndef VIDEO_PLAYER_VIDEO_RENDERING_OPENGL_API_SET_H
#define VIDEO_PLAYER_VIDEO_RENDERING_OPENGL_API_SET_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

struct mpv_opengl_init_params;

class VideoRenderingOpenGLAPISet {
public:
    explicit VideoRenderingOpenGLAPISet(const mpv_opengl_init_params *openglImplementationInfo);
    ~VideoRenderingOpenGLAPISet();

    VideoRenderingOpenGLAPISet(const VideoRenderingOpenGLAPISet &other) = delete;
    VideoRenderingOpenGLAPISet &operator =(const VideoRenderingOpenGLAPISet &other) = delete;

    PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
    PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
    PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
    PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;

    PFNGLGENTEXTURESPROC glGenTextures;
    PFNGLDELETETEXTURESPROC glDeleteTextures;
    PFNGLACTIVETEXTUREPROC glActiveTexture;
    PFNGLBINDTEXTUREPROC glBindTexture;
    PFNGLTEXIMAGE2DPROC glTexImage2D;
    PFNGLTEXPARAMETERIPROC glTexParameteri;

    PFNGLLABELOBJECTEXTPROC glLabelObjectEXT;

private:
    template<typename Result, typename... Args>
    static inline void bindFunction(const mpv_opengl_init_params *openglImplementationInfo, const char *name, Result (*&pointer)(Args... args),
                                    bool optional = false) {
        pointer = reinterpret_cast<Result (*)(Args...)>(bindFunction(openglImplementationInfo, name, optional));
    }

    static void *bindFunction(const mpv_opengl_init_params *openglImplementationInfo, const char *name, bool optional = false);

};

#endif

