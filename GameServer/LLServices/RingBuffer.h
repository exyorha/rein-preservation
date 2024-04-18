#ifndef LLSERVICES_RING_BUFFER_H
#define LLSERVICES_RING_BUFFER_H

#include <string.h>
#include <inttypes.h>
#include <array>
#include <utility>

namespace LLServices {
    class RingBuffer {
    public:
        using Pointer = intptr_t;

        enum class TransferType {
            Normal,
            Peek,
            Transactional
        };

        RingBuffer(unsigned char *storage, size_t size);
        ~RingBuffer();

        RingBuffer(const RingBuffer &other) = delete;
        RingBuffer &operator =(const RingBuffer &other) = delete;

        inline size_t size() const {
            return m_size;
        }

        void reset();

        size_t bytesAvailableForRead() const;

        inline size_t bytesAvailableForWrite() const {
            return bytesAvailableForWrite(m_readPointer);
        }

        size_t bytesAvailableForWrite(Pointer readPointer) const;

        size_t writeData(const unsigned char *data, size_t dataSize, TransferType mode);
        size_t readData(unsigned char *data, size_t dataSize, TransferType mode);

        unsigned char* getRangeForWrite(size_t& size);
        const unsigned char *getRangeForRead(size_t &size) const;

        using ConstSegment = std::pair<const unsigned char *, size_t>;
        using Segment = std::pair<unsigned char *, size_t>;

        inline void getSegmentsForRead(std::array<ConstSegment, 2> &segments) {
            getSegmentsForRead(segments, m_readPointer);
        }

        void getSegmentsForRead(std::array<ConstSegment, 2> &segments, Pointer readPointer);
        void getSegmentsForWrite(std::array<Segment, 2>& segments, size_t maxSize);

        inline Pointer writePointer() const {
            return m_writePointer;
        }

    private:
        const unsigned char *getRangeForReadInternal(size_t &size, Pointer readPointer) const;
        unsigned char* getRangeForWriteInternal(size_t& size, Pointer writePointer);

        unsigned char *m_data;
        size_t m_size;
        Pointer m_readPointer;
        Pointer m_writePointer;
    };
}

#endif
