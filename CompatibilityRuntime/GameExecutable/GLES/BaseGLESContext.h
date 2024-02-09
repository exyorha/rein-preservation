#ifndef GLES_BASE_GLES_CONTEXT_H
#define GLES_BASE_GLES_CONTEXT_H

class BaseGLESContext {
protected:
    BaseGLESContext();

public:
    virtual ~BaseGLESContext();

    BaseGLESContext(const BaseGLESContext &other) = delete;
    BaseGLESContext &operator =(const BaseGLESContext &other) = delete;

    virtual void *getProcAddress(const char *name) noexcept = 0;
};

#endif
