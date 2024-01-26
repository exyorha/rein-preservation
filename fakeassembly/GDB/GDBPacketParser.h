#ifndef GDB_PACKET_PARSER_H
#define GDB_PACKET_PARSER_H

#include <string_view>
#include <charconv>
#include <stdexcept>
#include <vector>

class GDBPacketParser {
public:
    explicit GDBPacketParser(const std::string_view &packet);
    ~GDBPacketParser();

    GDBPacketParser(const GDBPacketParser &other) = delete;
    GDBPacketParser &operator =(const GDBPacketParser &other) = delete;

    inline bool atEnd() const {
        return m_tail.empty();
    }

    char getCharacter();

    inline std::string_view getColonDelimited() {
        return getDelimited(':');
    }

    inline std::string_view getCommaDelimited() {
        return getDelimited(',');
    }

    std::string_view getRest();

    template<typename T>
    static T parseInteger(const std::string_view &string) {
        T value;

        auto result = std::from_chars(string.data(), string.data() + string.size(), value, 16);
        if(result.ec != std::errc() || result.ptr != string.data() + string.size() || string.empty())
            throw std::runtime_error("failed to parse an integer in a GDB packet");

        return value;
    }

    static std::vector<unsigned char> parseHex(const std::string_view &string);

private:
    std::string_view getDelimited(char delim);

    std::string_view m_tail;
};

#endif
