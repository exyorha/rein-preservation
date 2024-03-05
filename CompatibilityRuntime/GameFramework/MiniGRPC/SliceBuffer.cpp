#include "SliceBuffer.h"

#include <cstdio>

namespace minigrpc {

    SliceBuffer::SliceBuffer() = default;

    SliceBuffer::~SliceBuffer() = default;

    void SliceBuffer::reset() {
        m_buffer.clear();
    }

    unsigned char *SliceBuffer::adjustTailSpace(size_t available, size_t requested) {
        m_buffer.resize(m_buffer.size() - available + requested);

        return m_buffer.data() + m_buffer.size() - requested;
    }
}

