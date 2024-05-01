#ifndef WEBVIEW_CEF_COMPOSITOR_H
#define WEBVIEW_CEF_COMPOSITOR_H

#include <vector>

#include <GLES/GLESObjectHandle.h>

class CEFSurface;

class CEFCompositor {
public:
    CEFCompositor();
    ~CEFCompositor();

    CEFCompositor(const CEFCompositor &other) = delete;
    CEFCompositor &operator =(const CEFCompositor &other) = delete;

    void uploadSurface(CEFSurface *surface);
    void renderSurface(CEFSurface *surface);

    void scheduleTextureDisposal(GLESTextureHandle &&handle);
    void disposeStaleTextures();

    void beforeRender();
    void afterRender();

private:
    static GLESShaderHandle createShader(GLenum type, const std::string_view &label, const std::string_view &code);
    static void linkProgram(GLuint program, const std::string_view &labelForErrorMessages);

    GLESVertexArrayHandle m_vao;
    GLESProgramHandle m_program;
    std::vector<GLESTextureHandle> m_disposalQueue;

    GLint m_oldDrawFramebuffer;
    GLint m_oldReadFramebuffer;
    GLboolean m_oldBlend;
    GLboolean m_oldCullFace;
    GLboolean m_oldDepthTest;
    GLboolean m_oldRasterizerDiscard;
    GLboolean m_oldScissorTest;
    GLboolean m_oldStencilTest;
    GLint m_oldTextureUnit;
    GLint m_oldTexture2D;
    GLint m_oldVAO;
    GLint m_oldProgram;

    static const std::string_view m_vertexShaderCode;
    static const std::string_view m_fragmentShaderCode;
};

#endif
