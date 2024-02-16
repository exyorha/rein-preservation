#ifndef GLES_WGL_WGL_IMPLEMENTATION_NATIVE_H
#define GLES_WGL_WGL_IMPLEMENTATION_NATIVE_H

#include <GLES/WGL/WGLImplementation.h>
#include <GLES/BaseGLESContext.h>

class WGLImplementationNative final : public WGLImplementation {
public:
    WGLImplementationNative();
    ~WGLImplementationNative();

protected:
    std::unique_ptr<BaseGLESContext> CreateContextImpl(HDC hDC) override;
    std::unique_ptr<BaseGLESContext> CreateContextImpl(HDC hDC, BaseGLESContext *hShareContext,
                                                               const int *attribList) override;
    BOOL MakeCurrentImpl(HDC hDc, BaseGLESContext *newContext) noexcept override;

    PROC GetWGLProcAddress(LPCSTR proc) noexcept override;

private:
    class NativeGLESContext final : public BaseGLESContext {
    public:
        explicit NativeGLESContext(HGLRC context);
        ~NativeGLESContext() override;

        void *getProcAddress(const char *name) noexcept override;

        inline HGLRC context() const {
            return m_context;
        }

    private:
        HGLRC m_context;
    };
};

#endif
