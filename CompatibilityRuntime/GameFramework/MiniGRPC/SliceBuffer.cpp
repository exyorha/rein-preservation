#include "SliceBuffer.h"

#include <cstdio>

namespace minigrpc {

    SliceBuffer::SliceBuffer() {
        printf("SliceBuffer: created: %p\n", this);
    }

    SliceBuffer::~SliceBuffer() {
        printf("SliceBuffer: destroyed: %p\n", this);
    }

    void SliceBuffer::reset() {
        printf("SliceBuffer: reset\n");

        m_buffer.clear();
    }

    unsigned char *SliceBuffer::adjustTailSpace(size_t available, size_t requested) {
        printf("SliceBuffer(%p)::adjustTailSpace(%zu, %zu) (buffer size: %zu)\n", this, available, requested, m_buffer.size());

        m_buffer.resize(m_buffer.size() - available + requested);

        return m_buffer.data() + m_buffer.size() - requested;
    }
}

