#include <WebView/CEFCompositor.h>
#include <WebView/CEFSurface.h>

#include <GLES/GLESAPISet.h>

#include <sstream>

CEFCompositor::CEFCompositor() {
    m_vao = GLESVertexArrayHandle::create();
    m_vao.label("CEFCompositor surface VAO");

    auto vertexShader = createShader(GL_VERTEX_SHADER, "CEFCompositor surface vertex shader", m_vertexShaderCode);
    auto fragmentShader = createShader(GL_FRAGMENT_SHADER, "CEFCompositor surface fragment shader", m_fragmentShaderCode);

    m_program = GLESProgramHandle::create();
    m_program.label("CEFCompositor surface program");

    const auto &api = GLESAPISet::get();
    api.glAttachShader(m_program, vertexShader);
    api.glAttachShader(m_program, fragmentShader);

    linkProgram(m_program, "CEFCompositor surface program");

    api.glDetachShader(m_program, vertexShader);
    api.glDetachShader(m_program, fragmentShader);

}

GLESShaderHandle CEFCompositor::createShader(GLenum type, const std::string_view &label, const std::string_view &code) {
    const auto &api = GLESAPISet::get();

    auto shader = GLESShaderHandle::create(type);
    if(!label.empty()) {
        shader.label(label);
    }

    const GLchar *codePtr = code.data();
    GLint codeLength = code.size();

    api.glShaderSource(shader, 1, &codePtr, &codeLength);

    api.glCompileShader(shader);

    GLint infoLogLength = 0;
    api.glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

    if(infoLogLength != 0) {
        std::vector<GLchar> infoLog(infoLogLength);

        GLsizei written = 0;
        api.glGetShaderInfoLog(shader, infoLog.size(), &written, infoLog.data());

        fprintf(stderr, "CEFCompositor::createShader: messages from the compilation of '%.*s':\n",
                static_cast<int>(label.size()), label.data());

        fwrite(infoLog.data(), 1, written, stderr);
        fputs("\n", stderr);
    }

    GLint status;
    api.glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE) {
        std::stringstream error;
        error << "Shader '" << label << "' has failed to compile";
        throw std::runtime_error(error.str());
    }

    return shader;
}

void CEFCompositor::linkProgram(GLuint program, const std::string_view &labelForErrorMessages) {
    const auto &api = GLESAPISet::get();

    api.glLinkProgram(program);

    GLint infoLogLength = 0;
    api.glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

    if(infoLogLength != 0) {
        std::vector<GLchar> infoLog(infoLogLength);

        GLsizei written = 0;
        api.glGetProgramInfoLog(program, infoLog.size(), &written, infoLog.data());

        fprintf(stderr, "CEFCompositor::linkProgram: messages from the linking of '%.*s':\n",
                static_cast<int>(labelForErrorMessages.size()), labelForErrorMessages.data());

        fwrite(infoLog.data(), 1, written, stderr);
        fputs("\n", stderr);
    }

    GLint status;
    api.glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status != GL_TRUE) {
        std::stringstream error;
        error << "Program '" << labelForErrorMessages << "' has failed to compile";
        throw std::runtime_error(error.str());
    }
}

CEFCompositor::~CEFCompositor() = default;

void CEFCompositor::uploadSurface(CEFSurface *surface) {
    surface->upload(this);
}


void CEFCompositor::scheduleTextureDisposal(GLESTextureHandle &&handle) {
    m_disposalQueue.emplace_back(std::move(handle));
}

void CEFCompositor::disposeStaleTextures() {
    m_disposalQueue.clear();
}

void CEFCompositor::beforeRender() {
    const auto &api = GLESAPISet::get();

    api.glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_oldDrawFramebuffer);
    api.glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_oldReadFramebuffer);
    api.glBindFramebuffer(GL_FRAMEBUFFER, 0);

    api.glGetBooleanv(GL_BLEND, &m_oldBlend);
    api.glEnable(GL_BLEND);
    api.glGetBooleanv(GL_CULL_FACE, &m_oldCullFace);
    api.glDisable(GL_CULL_FACE);
    api.glGetBooleanv(GL_DEPTH_TEST, &m_oldDepthTest);
    api.glDisable(GL_DEPTH_TEST);
    api.glGetBooleanv(GL_RASTERIZER_DISCARD, &m_oldRasterizerDiscard);
    api.glDisable(GL_RASTERIZER_DISCARD);
    api.glGetBooleanv(GL_SCISSOR_TEST, &m_oldScissorTest);
    api.glDisable(GL_SCISSOR_TEST);
    api.glGetBooleanv(GL_STENCIL_TEST, &m_oldStencilTest);
    api.glDisable(GL_STENCIL_TEST);

    api.glGetIntegerv(GL_ACTIVE_TEXTURE, &m_oldTextureUnit);
    api.glActiveTexture(GL_TEXTURE0);

    api.glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_oldTexture2D);

    api.glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &m_oldVAO);
    api.glBindVertexArray(m_vao);

    api.glGetIntegerv(GL_CURRENT_PROGRAM, &m_oldProgram);
    api.glUseProgram(m_program);
}

void CEFCompositor::afterRender() {
    const auto &api = GLESAPISet::get();

    api.glUseProgram(m_oldProgram);
    api.glBindTexture(GL_TEXTURE_2D, m_oldTexture2D);
    api.glActiveTexture(m_oldTextureUnit);
    api.glBindVertexArray(m_oldVAO);

    api.setEnabled(GL_BLEND, m_oldBlend);
    api.setEnabled(GL_CULL_FACE, m_oldCullFace);
    api.setEnabled(GL_DEPTH_TEST, m_oldDepthTest);
    api.setEnabled(GL_RASTERIZER_DISCARD, m_oldRasterizerDiscard);
    api.setEnabled(GL_SCISSOR_TEST, m_oldScissorTest);
    api.setEnabled(GL_STENCIL_TEST, m_oldStencilTest);

    GLboolean m_oldCullFace;
    GLboolean m_oldDepthTest;
    GLboolean m_oldRasterizerDiscard;
    GLboolean m_oldScissorTest;
    GLboolean m_oldStencilTest;

    api.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_oldDrawFramebuffer);
    api.glBindFramebuffer(GL_READ_FRAMEBUFFER, m_oldReadFramebuffer);
}

void CEFCompositor::renderSurface(CEFSurface *surface) {
    if(!surface->texture())
        return;

    const auto &api = GLESAPISet::get();

    api.glViewport(surface->x(), surface->y(), surface->width(), surface->height());
    api.glBindTexture(GL_TEXTURE_2D, surface->texture());
    api.glDrawArrays(GL_TRIANGLES, 0, 3);
}

using namespace std::string_view_literals;

const std::string_view CEFCompositor::m_vertexShaderCode = R"GLSL(
#version 130

out vec2 texcoord;

void main() {
    const vec2 texcoordTable[3] = vec2[3](
        vec2(0, 0),
        vec2(2, 0),
        vec2(0, 2)
    );
    texcoord = texcoordTable[gl_VertexID];
    gl_Position = vec4(2 * texcoord - 1, 0, 1);
}
)GLSL"sv;

const std::string_view CEFCompositor::m_fragmentShaderCode = R"GLSL(
#version 130

in vec2 texcoord;

uniform sampler2D surfaceTexture;

out vec4 color;

void main() {
    color = texture(surfaceTexture, vec2(texcoord.x, 1.0 - texcoord.y)).bgra;
}
)GLSL"sv;
