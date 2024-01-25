#include "GDBPacketParser.h"

#include <stdexcept>

GDBPacketParser::GDBPacketParser(const std::string_view &packet) : m_tail(packet) {

}

GDBPacketParser::~GDBPacketParser() = default;

char GDBPacketParser::getCharacter() {
    if(m_tail.empty())
        throw std::logic_error("already at the end of packet");

    auto result = m_tail.front();

    m_tail = m_tail.substr(1);

    return result;
}

std::string_view GDBPacketParser::getRest() {
    auto result = m_tail;
    m_tail = std::string_view();

    return result;
}

std::string_view GDBPacketParser::getDelimited(char delimChar) {
    auto delim = m_tail.find(delimChar);
    if(delim != std::string::npos) {
        auto result = m_tail.substr(0, delim);
        m_tail = m_tail.substr(delim + 1);

        return result;
    } else {
        auto result = m_tail;
        m_tail = std::string_view();
        return result;
    }
}

std::vector<unsigned char> GDBPacketParser::parseHex(const std::string_view &content) {
    if(content.size() % 2)
        throw std::logic_error("hex content has odd length");

    auto bytes = content.size() / 2;
    std::vector<unsigned char> data;

    data.resize(bytes);

    auto ptr = data.data();

    for(auto it = content.begin(), end = content.end(); it != end; it += 2) {
        auto cbegin = &*it;
        auto cend = &*(it + 2);

        auto result = std::from_chars(cbegin, cend, *ptr, 16);
        if(result.ec != std::errc() || result.ptr != cend)
            throw std::logic_error("invalid hex value");

        ptr++;
    }

    return data;
}
