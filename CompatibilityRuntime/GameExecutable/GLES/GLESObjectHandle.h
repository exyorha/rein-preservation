#ifndef GLES_GLES_OBJECT_HANDLE_H
#define GLES_GLES_OBJECT_HANDLE_H

#include <GLES/GLESAPISet.h>

#include <utility>
#include <string_view>

template<typename Traits>
class GLESObjectHandle {
public:
    inline GLESObjectHandle() noexcept : GLESObjectHandle(0) {}
    inline explicit GLESObjectHandle(GLuint handle) noexcept : m_handle(handle) {}

    inline ~GLESObjectHandle() {
        if(m_handle != 0) {
            Traits::freeObject(m_handle);
        }
    }

    GLESObjectHandle(const GLESObjectHandle &other) = delete;
    GLESObjectHandle &operator =(const GLESObjectHandle &other) = delete;

    inline GLESObjectHandle(GLESObjectHandle &&other) noexcept : GLESObjectHandle() {
        *this = std::move(other);
    }

    GLESObjectHandle &operator =(GLESObjectHandle &&other) noexcept {
        if(this != &other) {
            if(m_handle != 0) {
                Traits::freeObject(m_handle);
            }

            m_handle = other.m_handle;
            other.m_handle = 0;
        }

        return *this;
    }

    inline operator bool() const {
        return m_handle != 0;
    }

    inline operator GLuint() const {
        return m_handle;
    }

    inline GLuint handle() const {
        return m_handle;
    }

    template<typename... Args>
    static inline GLESObjectHandle create(Args... args) {
        return GLESObjectHandle(Traits::createObject(args...));
    }

    inline void label(const std::string_view &name) {
        if(m_handle != 0) {

            const auto &api = GLESAPISet::get();
            if(api.glLabelObjectEXT) {
                api.glLabelObjectEXT(Traits::LabelObjectType, m_handle, name.size(), name.data());
            }
        }

    }

private:
    GLuint m_handle;
};

struct GLESTextureTraits {
    static constexpr GLenum LabelObjectType = GL_TEXTURE;

    static inline GLuint createObject() {
        GLuint handle;
        GLESAPISet::get().glGenTextures(1, &handle);
        return handle;
    }

    static inline void freeObject(GLuint handle) {
        GLESAPISet::get().glDeleteTextures(1, &handle);
    }
};

struct GLESFramebufferTraits {
    static constexpr GLenum LabelObjectType = GL_FRAMEBUFFER;

    static inline GLuint createObject() {
        GLuint handle;
        GLESAPISet::get().glGenFramebuffers(1, &handle);
        return handle;
    }

    static inline void freeObject(GLuint handle) {
        GLESAPISet::get().glDeleteFramebuffers(1, &handle);
    }
};

struct GLESVertexArrayTraits {
    static constexpr GLenum LabelObjectType = GL_VERTEX_ARRAY;

    static inline GLuint createObject() {
        GLuint handle;
        GLESAPISet::get().glGenVertexArrays(1, &handle);
        return handle;
    }

    static inline void freeObject(GLuint handle) {
        GLESAPISet::get().glDeleteVertexArrays(1, &handle);
    }
};

struct GLESProgramTraits {
    static constexpr GLenum LabelObjectType = GL_PROGRAM;

    static inline GLuint createObject() {
        return GLESAPISet::get().glCreateProgram();
    }

    static inline void freeObject(GLuint handle) {
        GLESAPISet::get().glDeleteProgram(handle);
    }
};

struct GLESShaderTraits {
    static constexpr GLenum LabelObjectType = GL_PROGRAM;

    static inline GLuint createObject(GLenum type) {
        return GLESAPISet::get().glCreateShader(type);
    }

    static inline void freeObject(GLuint handle) {
        GLESAPISet::get().glDeleteShader(handle);
    }
};

using GLESTextureHandle = GLESObjectHandle<GLESTextureTraits>;
using GLESFramebufferHandle = GLESObjectHandle<GLESFramebufferTraits>;
using GLESVertexArrayHandle = GLESObjectHandle<GLESVertexArrayTraits>;
using GLESProgramHandle = GLESObjectHandle<GLESProgramTraits>;
using GLESShaderHandle = GLESObjectHandle<GLESShaderTraits>;

#endif
