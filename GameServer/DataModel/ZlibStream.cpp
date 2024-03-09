#include <DataModel/ZlibStream.h>
#include <DataModel/ZlibException.h>

#include <cstring>
#include <cstdlib>
#include <stdexcept>

ZlibStream::ZlibStream() : m_mode(Mode::None) {
    memset(static_cast<z_stream *>(this), 0, sizeof(z_stream));

    zalloc = zlibAlloc;
    zfree = zlibFree;
}

ZlibStream::~ZlibStream() {
    if(m_mode == Mode::Inflate) {
        inflateEnd(this);
    } else if(m_mode == Mode::Deflate) {
        deflateEnd(this);
    }
}

void ZlibStream::initInflate(int windowBits) {
    if(m_mode != Mode::None)
        throw std::logic_error("ZlibStream::initInflate: this stream is already in use");

    auto result = inflateInit2(this, windowBits);
    if(result != Z_OK)
        throw ZlibException(result, *this);

    m_mode = Mode::Inflate;
}

void ZlibStream::initDeflate(int level, int windowSize) {
    if(m_mode != Mode::None)
        throw std::logic_error("ZlibStream::initDeflate: this stream is already in use");

    auto result = deflateInit2(this, level, Z_DEFLATED, windowSize, 8, Z_DEFAULT_STRATEGY);
    if(result != Z_OK)
        throw ZlibException(result, *this);

    m_mode = Mode::Deflate;
}

voidpf ZlibStream::zlibAlloc(voidpf opaque, uInt items, uInt size) {
    (void)opaque;

    return malloc(static_cast<size_t>(items) * size);
}

void ZlibStream::zlibFree(voidpf opaque, voidpf address) {
    (void)opaque;

    free(address);
}
