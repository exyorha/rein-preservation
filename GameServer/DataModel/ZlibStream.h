#ifndef DATA_MODEL_ZLIB_STREAM_H
#define DATA_MODEL_ZLIB_STREAM_H

#include <zlib.h>

class ZlibStream final : public z_stream {
public:
    ZlibStream();
    ~ZlibStream();

    ZlibStream(const ZlibStream &other) = delete;
    ZlibStream &operator =(const ZlibStream &other) = delete;

    void initDeflate(int level = Z_DEFAULT_COMPRESSION, int windowBits = 15);
    void initInflate(int windowBits = 15);

private:
    static voidpf zlibAlloc(voidpf opaque, uInt items, uInt size);
    static void zlibFree(voidpf opaque, voidpf address);

    enum class Mode {
        None,
        Inflate,
        Deflate
    };

    Mode m_mode;
};

#endif
