#include "event2/buffer.h"
#include <GRPC/PBInputOverLLBuffer.h>

PBInputOverLLBuffer::PBInputOverLLBuffer(evbuffer *buffer) : m_buffer(buffer), m_pos(0) {

}

PBInputOverLLBuffer::~PBInputOverLLBuffer() = default;

bool PBInputOverLLBuffer::Next(const void** data, int* size) {
    evbuffer_ptr position;
    if(evbuffer_ptr_set(m_buffer, &position, m_pos, EVBUFFER_PTR_SET) < 0)
        throw std::logic_error("evbuffer_ptr_set has failed");

    struct evbuffer_iovec vec;

    if(evbuffer_peek(m_buffer, -1, &position, &vec, 1) <= 0) {
        return false;
    }

    *data = vec.iov_base;
    *size = vec.iov_len;

    m_pos += vec.iov_len;

    return true;
}

void PBInputOverLLBuffer::BackUp(int count) {
    m_pos -= count;
}

bool PBInputOverLLBuffer::Skip(int count) {
    if(m_pos + count <= evbuffer_get_length(m_buffer)) {
        m_pos += count;

        return true;
    } else {
        return false;
    }
}

int64_t PBInputOverLLBuffer::ByteCount() const {
    return m_pos;
}
