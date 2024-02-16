#ifndef GLES_SHIM_SHIM_NEXT_CONTEXT_SYMBOLS_H
#define GLES_SHIM_SHIM_NEXT_CONTEXT_SYMBOLS_H

#define GL_GLES_PROTOTYPES 0
#include <GLES3/gl32.h>

class BaseGLESContext;

class ShimNextContextSymbols {
public:
    explicit ShimNextContextSymbols(BaseGLESContext &context);
    ~ShimNextContextSymbols();

    ShimNextContextSymbols(const ShimNextContextSymbols &other) = delete;
    ShimNextContextSymbols &operator =(const ShimNextContextSymbols &other) = delete;

    PFNGLGETSTRINGPROC glGetString;
    PFNGLGETSTRINGIPROC glGetStringi;
    PFNGLGETINTEGERVPROC glGetIntegerv;
    PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
    PFNGLTEXIMAGE2DPROC glTexImage2D;
    PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;
    PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
    PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
    PFNGLTEXSTORAGE2DPROC glTexStorage2D;
    PFNGLTEXSTORAGE3DPROC glTexStorage3D;

};

#endif

