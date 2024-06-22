#ifndef SLICE_BUFFER_H
#define SLICE_BUFFER_H

#include <vector>
#include <cstring>

namespace minigrpc {

    class SliceBuffer {
    public:
        SliceBuffer();
        ~SliceBuffer();

        SliceBuffer(const SliceBuffer &other) = delete;
        SliceBuffer &operator =(const SliceBuffer &other) = delete;

        void reset();

        unsigned char *adjustTailSpace(size_t available, size_t requested);

        inline bool empty() const {
            return m_buffer.empty();
        }

        inline size_t size() const {
            return m_buffer.size();
        }

        inline const unsigned char *data() const {
            return m_buffer.data();
        }

        inline unsigned char *data() {
            return m_buffer.data();
        }

        inline const std::vector<unsigned char> &bytes() const {
            return m_buffer;
        }

        inline std::vector<unsigned char> &bytes() {
            return m_buffer;
        }

    private:
        std::vector<unsigned char> m_buffer;
    };

}

#endif
