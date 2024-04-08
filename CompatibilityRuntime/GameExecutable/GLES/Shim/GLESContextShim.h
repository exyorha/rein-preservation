#ifndef GLES_SHIM_GLES_CONTEXT_SHIM_H
#define GLES_SHIM_GLES_CONTEXT_SHIM_H

#include "GLES/TextureEmulation/EmulatedTextureFormat.h"
#include <GLES/BaseGLESContext.h>

#include <GLES/Shim/ShimNextContextSymbols.h>
#include <GLES/Shim/ShimExtensionString.h>

#include <memory>
#include <optional>

class GLESContextShim final : public BaseGLESContext {
public:
    explicit GLESContextShim(std::unique_ptr<BaseGLESContext> &&nextContext);
    ~GLESContextShim() override;

    inline BaseGLESContext *nextContext() const {
        return m_nextContext.get();
    }

    template<typename T>
    static inline BaseGLESContext *unwrap(T context) {
        if(!context)
            return nullptr;

        return reinterpret_cast<GLESContextShim *>(context)->nextContext();
    }

    void *getProcAddress(const char *name) noexcept override;

    static bool AlwaysEmulateASTC;
    static bool NeverRecompressASTC;

private:
    void lateInitialize();

    static GLESContextShim *getCurrentShim();

    static const GLubyte *GL_APIENTRY shim_glGetString(GLenum name);
    static const GLubyte *GL_APIENTRY shim_glGetStringi(GLenum name, GLuint index);
    static void GL_APIENTRY shim_glGetIntegerv(GLenum pname, GLint *data);
    static void GL_APIENTRY shim_glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width,
                                                         GLsizei height, GLint border, GLsizei imageSize, const void *data);

    static void GL_APIENTRY shim_glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                                                            GLsizei height, GLenum format, GLsizei imageSize, const void *data);

    static void GL_APIENTRY shim_glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
                                                            GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);

    static void GL_APIENTRY shim_glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
    static void GL_APIENTRY shim_glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);

    GLenum subsituteStorageInternalFormat(GLenum internalformat, bool canCompress) const;
    static bool acceptableImageDimensionsForCompression(unsigned int width, unsigned int height);

    std::vector<unsigned char> decompressTexture(const EmulatedTextureFormat *emulation, GLsizei width, GLsizei height, const void *data);
    static std::vector<unsigned char> compressToDXT5(GLsizei width, GLsizei height, const unsigned char *rgbaData);

    static GLESContextShim *getAndInitializeShim();

    std::unique_ptr<BaseGLESContext> m_nextContext;
    std::optional<ShimNextContextSymbols> m_nextSymbols;
    std::optional<ShimExtensionString> m_extensionString;
    bool m_emulatedASTC;
    bool m_recompressASTC;
    bool m_warnedAboutEmulatingTextures;
};

#endif

