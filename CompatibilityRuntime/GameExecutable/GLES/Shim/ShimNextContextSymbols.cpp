#include <GLES/Shim/ShimNextContextSymbols.h>

#include <GLES/BaseGLESContext.h>

#include <stdexcept>

ShimNextContextSymbols::ShimNextContextSymbols(BaseGLESContext &context) {
    glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(context.getProcAddress("glGetString"));
    if(!glGetString)
        throw std::runtime_error("failed to bind glGetString");

    glGetStringi = reinterpret_cast<PFNGLGETSTRINGIPROC>(context.getProcAddress("glGetStringi"));
    if(!glGetStringi)
        throw std::runtime_error("failed to bind glGetStringi");

    glGetIntegerv = reinterpret_cast<PFNGLGETINTEGERVPROC>(context.getProcAddress("glGetIntegerv"));
    if(!glGetIntegerv)
        throw std::runtime_error("failed to bind glGetIntegerv");

    glCompressedTexImage2D = reinterpret_cast<PFNGLCOMPRESSEDTEXIMAGE2DPROC>(context.getProcAddress("glCompressedTexImage2D"));
    if(!glCompressedTexImage2D)
        throw std::runtime_error("failed to bind glCompressedTexImage2D");

    glTexImage2D = reinterpret_cast<PFNGLTEXIMAGE2DPROC>(context.getProcAddress("glTexImage2D"));
    if(!glTexImage2D)
        throw std::runtime_error("failed to bind glTexImage2D");

    glCompressedTexSubImage2D = reinterpret_cast<PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC>(context.getProcAddress("glCompressedTexSubImage2D"));
    if(!glCompressedTexSubImage2D)
        throw std::runtime_error("failed to bind glCompressedTexSubImage2D");

    glTexSubImage2D = reinterpret_cast<PFNGLTEXSUBIMAGE2DPROC>(context.getProcAddress("glTexSubImage2D"));
    if(!glTexSubImage2D)
        throw std::runtime_error("failed to bind glTexSubImage2D");

    glCompressedTexSubImage3D = reinterpret_cast<PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC>(context.getProcAddress("glCompressedTexSubImage3D"));
    if(!glCompressedTexSubImage3D)
        throw std::runtime_error("failed to bind glCompressedTexSubImage2D");

    glTexStorage2D = reinterpret_cast<PFNGLTEXSTORAGE2DPROC>(context.getProcAddress("glTexStorage2D"));
    if(!glTexStorage2D)
        throw std::runtime_error("failed to bind glTexStorage2D");

    glTexStorage3D = reinterpret_cast<PFNGLTEXSTORAGE3DPROC>(context.getProcAddress("glTexStorage3D"));
    if(!glTexStorage3D)
        throw std::runtime_error("failed to bind glTexStorage3D");
}

ShimNextContextSymbols::~ShimNextContextSymbols() = default;
