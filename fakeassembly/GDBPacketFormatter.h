#ifndef GDB_PACKET_FORMATTER_H
#define GDB_PACKET_FORMATTER_H

#include <string>

class GDBPacketFormatter {
public:
    GDBPacketFormatter();
    ~GDBPacketFormatter();

    GDBPacketFormatter(const GDBPacketFormatter &other) = delete;
    GDBPacketFormatter &operator =(const GDBPacketFormatter &other) = delete;

    inline const std::string &string() const {
        return m_string;
    }

    void hex(const void *data, size_t size);

    void text(const std::string_view &text);

    void error(int error);

private:
    std::string m_string;
};

#endif
