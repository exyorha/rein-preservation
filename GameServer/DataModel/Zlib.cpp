#include <DataModel/Zlib.h>
#include <DataModel/ZlibException.h>
#include <DataModel/ZlibStream.h>

#include <zlib.h>

void inflate(const unsigned char *source, size_t sourceLength, unsigned char *destination, size_t destinationLength) {
    auto sourceLen = static_cast<unsigned long>(sourceLength);
    auto destinationLen = static_cast<unsigned long>(destinationLength);

    int result = uncompress2(destination, &destinationLen, source, &sourceLen);

    if(result < 0) {
        throw ZlibException(result);
    }

    if(sourceLen != sourceLength) {
        throw std::runtime_error("inflate: the input stream didn't occupy the whole input length");
    }

    if(destinationLen != destinationLength) {
        throw std::runtime_error("inflate: the output stream didn't occupy the whole output length");
    }
}

static std::string runStream(ZlibStream &stream, int (*ZEXPORT runner)(z_streamp stream, int flush), int flush) {
    std::string backingBuffer;

    while(true) {
        stream.avail_out = backingBuffer.size() - stream.total_out;

        if(stream.avail_out == 0) {
            backingBuffer.resize(stream.total_out + 32768);
            stream.avail_out = backingBuffer.size() - stream.total_out;
        }

        stream.next_out = (Bytef *)backingBuffer.data() + stream.total_out;

        auto result = runner(&stream, flush);
        if(result == Z_STREAM_END)
            break;

        if(result != Z_OK)
            throw ZlibException(result, stream);
    }

    backingBuffer.resize(stream.total_out);

    return backingBuffer;
}

std::string inflate(const std::string_view &source, bool gzip) {
    ZlibStream stream;
    stream.next_in = (Bytef *)source.data();
    stream.avail_in = source.size();

    int windowBits = 15;
    if(gzip)
        windowBits += 16;

    stream.initInflate(windowBits);

    return runStream(stream, ::inflate, Z_NO_FLUSH);
}

std::string deflate(const std::string_view &source, bool gzip) {
    ZlibStream stream;
    stream.next_in = (Bytef *)source.data();
    stream.avail_in = source.size();

    int windowBits = 15;
    if(gzip)
        windowBits += 16;

    stream.initDeflate(Z_DEFAULT_COMPRESSION, windowBits);

    return runStream(stream, ::deflate, Z_FINISH);
}

