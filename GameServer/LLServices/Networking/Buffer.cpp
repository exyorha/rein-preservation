#include <LLServices/Networking/Buffer.h>

namespace LLServices {

    Buffer::Buffer() {
        auto buffer = evbuffer_new();
        if(!buffer)
            throw std::bad_alloc();

        m_buffer.reset(buffer);
    }

    Buffer::~Buffer() = default;

    Buffer::Buffer(Buffer &&other) noexcept = default;

    Buffer &Buffer::operator =(Buffer &&other) noexcept = default;

    void Buffer::append(const void *data, size_t dataSize) {
        evbuffer_add(m_buffer.get(), data, dataSize);
    }
}

