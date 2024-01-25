#include "GDBPacketFormatter.h"

GDBPacketFormatter::GDBPacketFormatter() = default;

GDBPacketFormatter::~GDBPacketFormatter() = default;

void GDBPacketFormatter::hex(const void *data, size_t size) {
    auto bytes = static_cast<const unsigned char *>(data);

    auto reservation = m_string.size() + size * 2;
    if(m_string.capacity() < reservation)
        m_string.reserve(reservation);

    for(const unsigned char *ptr = bytes, *limit = ptr + size; ptr < limit; ptr++) {
        static const char hextable[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

        auto byte = *ptr;

        m_string.push_back(hextable[byte >> 4]);
        m_string.push_back(hextable[byte & 0x0F]);
    }

}

void GDBPacketFormatter::error(int error) {
    m_string.push_back('E');

    auto code = static_cast<unsigned char>(static_cast<unsigned int>(error));

    hex(&code, sizeof(code));
}

void GDBPacketFormatter::text(const std::string_view &text) {
    m_string.append(text);
}
