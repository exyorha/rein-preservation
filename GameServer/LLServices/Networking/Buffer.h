#ifndef LLSERVICES_NETWORKING_BUFFER_H
#define LLSERVICES_NETWORKING_BUFFER_H

#include "LLServices/Networking/EventTypes.h"

#include <string_view>

namespace LLServices {

    class Buffer {
    public:
        Buffer();
        ~Buffer();

        Buffer(const Buffer &other) = delete;
        Buffer &operator =(const Buffer &other) = delete;

        Buffer(Buffer &&other) noexcept;
        Buffer &operator =(Buffer &&other) noexcept;

        inline evbuffer *get() {
            return m_buffer.get();
        }

        inline const evbuffer *get() const {
            return m_buffer.get();
        }

        void append(const void *data, size_t dataSize);

        inline void append(const std::string_view &string) {
            append(string.data(), string.size());
        }

    private:
        BufferPtr m_buffer;
    };

}

#endif
