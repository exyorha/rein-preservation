#include <LLServices/RingBuffer.h>

#include <stdint.h>

namespace LLServices {
    RingBuffer::RingBuffer(unsigned char *data, size_t size) : m_data(data), m_size(size), m_readPointer(0), m_writePointer(0) {

    }

    RingBuffer::~RingBuffer() = default;

    void RingBuffer::reset() {
        m_readPointer = 0;
        m_writePointer = 0;
    }

    size_t RingBuffer::bytesAvailableForRead() const {
        Pointer bytesToRead = m_writePointer - m_readPointer;

        if (bytesToRead < 0) {
            return m_size + bytesToRead;
        }
        else {
            return bytesToRead;
        }
    }

    size_t RingBuffer::bytesAvailableForWrite() const {
        Pointer bytesToWrite = m_readPointer - m_writePointer - 1;

        if (bytesToWrite < 0) {
            return m_size + bytesToWrite;
        }
        else {
            return bytesToWrite;
        }
    }

    size_t RingBuffer::writeData(const unsigned char *data, size_t dataSize, TransferType mode) {
        size_t totalTransferred = 0;

        if (mode != TransferType::Transactional || bytesAvailableForWrite() >= dataSize) {

            size_t writePointer = m_writePointer;
            Pointer bytesToWrite;

            do {
                bytesToWrite = m_readPointer - writePointer - 1;
                if (bytesToWrite < 0) {
                    bytesToWrite += m_size;
                }

                Pointer untilWraparound = m_size - writePointer;
                if (bytesToWrite > untilWraparound)
                    bytesToWrite = untilWraparound;

                if (bytesToWrite > static_cast<Pointer>(dataSize))
                    bytesToWrite = static_cast<Pointer>(dataSize);

                totalTransferred += bytesToWrite;

                if (bytesToWrite > 0) {
                    if (data) {
                        memcpy(&m_data[writePointer], data, bytesToWrite);
                        data += bytesToWrite;
                    }

                    dataSize -= bytesToWrite;
                    writePointer += bytesToWrite;

                    if (writePointer == m_size) {
                        writePointer = 0;
                    }
                }
            } while (bytesToWrite > 0);

            if (mode != TransferType::Peek) {
                m_writePointer = writePointer;
            }
        }

        return totalTransferred;
    }

    size_t RingBuffer::readData(unsigned char *data, size_t dataSize, TransferType mode) {
        size_t bytesRead = 0;

        if (mode != TransferType::Transactional ||
            bytesAvailableForRead() >= dataSize) {

            size_t readPointer = m_readPointer;
            Pointer bytesRemaining;

            if (readPointer > m_writePointer) {
                bytesRemaining = m_size - readPointer;
                if (bytesRemaining > static_cast<Pointer>(dataSize)) {
                    bytesRemaining = static_cast<Pointer>(dataSize);
                }

                if (data) {
                    memcpy(data, &m_data[readPointer], bytesRemaining);
                    data += bytesRemaining;
                }

                bytesRead += bytesRemaining;
                dataSize -= bytesRemaining;
                readPointer += bytesRemaining;
                if (readPointer == m_size)
                    readPointer = 0;
            }

            bytesRemaining = m_writePointer - readPointer;
            if (bytesRemaining > static_cast<Pointer>(dataSize)) {
                bytesRemaining = static_cast<Pointer>(dataSize);
            }

            if (bytesRemaining > 0) {
                if (data) {
                    memcpy(data, &m_data[readPointer], bytesRemaining);
                }

                bytesRead += bytesRemaining;
                readPointer += bytesRemaining;
                if (readPointer == m_size)
                    readPointer = 0;
            }

            if (mode != TransferType::Peek) {
                m_readPointer = readPointer;
            }
        }

        return bytesRead;
    }

    unsigned char* RingBuffer::getRangeForWrite(size_t& size) {
        return getRangeForWriteInternal(size, m_writePointer);
    }

    const unsigned char *RingBuffer::getRangeForRead(size_t &size) const {
        return getRangeForReadInternal(size, m_readPointer);
    }

    void RingBuffer::getSegmentsForRead(std::array<ConstSegment, 2> &segments) {
        size_t len;
        auto ptr = getRangeForReadInternal(len, m_readPointer);
        segments[0] = std::make_pair(ptr, len);

        Pointer adjustedPointer = m_readPointer + len;
        if (adjustedPointer == size())
            adjustedPointer = 0;

        ptr = getRangeForReadInternal(len, adjustedPointer);
        segments[1] = std::make_pair(ptr, len);

    }

    void RingBuffer::getSegmentsForWrite(std::array<Segment, 2> &segments, size_t maxSize) {
        size_t len;
        auto ptr = getRangeForWriteInternal(len, m_writePointer);

        size_t advertisedLen = len;
        if (len > maxSize) {
            advertisedLen = maxSize;
        }

        segments[0] = std::make_pair(ptr, advertisedLen);

        if (advertisedLen == maxSize) {
            segments[1] = std::make_pair<unsigned char *, size_t>(ptr + advertisedLen, 0);
            return;
        }

        maxSize -= len;

        Pointer adjustedPointer = m_writePointer + len;
        if (adjustedPointer == size())
                adjustedPointer = 0;

        ptr = getRangeForWriteInternal(len, adjustedPointer);

        advertisedLen = len;

        if (len > maxSize) {
                advertisedLen = maxSize;
        }

        segments[1] = std::make_pair(ptr, advertisedLen);
    }

    const unsigned char *RingBuffer::getRangeForReadInternal(size_t &size, Pointer readPointer) const {
        size_t dataSize = 0;
        size_t dataOffset = 0;

        if (readPointer > m_writePointer) {
            dataSize = m_size - readPointer;
            dataOffset = readPointer;

            readPointer += dataSize;

            if (readPointer == m_size) {
                readPointer = 0;
            }
        }

        if (dataSize == 0) {
            dataSize = m_writePointer - readPointer;
            dataOffset = readPointer;
        }

        size = dataSize;
        return &m_data[dataOffset];
    }

    unsigned char* RingBuffer::getRangeForWriteInternal(size_t& size, Pointer writePointer) {
        auto dataSize = m_readPointer - writePointer - 1;

        auto raw = dataSize;

        if (dataSize < 0) {
            dataSize += m_size;
        }

        auto untilWraparound = m_size - writePointer;

        if (dataSize > untilWraparound) {
            dataSize = untilWraparound;
        }

        size = dataSize;
        return &m_data[writePointer];
    }

}

