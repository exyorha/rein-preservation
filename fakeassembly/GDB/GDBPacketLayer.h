#ifndef GDB_PACKET_LAYER_H
#define GDB_PACKET_LAYER_H

#include "FileDescriptor.h"

#include <string>

class GDBPacketLayer {
public:
    explicit GDBPacketLayer(const char *configuration);
    ~GDBPacketLayer();

    GDBPacketLayer(const GDBPacketLayer &other) = delete;
    GDBPacketLayer &operator =(const GDBPacketLayer &other) = delete;

    bool getInterrupt();

    std::string getPacket();

    void sendPacket(const std::string &packet = {});

    static inline size_t packetBufferSize() {
        return sizeof(m_packetBuffer);
    }

    inline void stopDoingAcks() {
        m_doingAcks = false;
    }

private:
    void pollFor(int event);

    void packetBufferConsume(size_t bytes);
    static unsigned char packetChecksum(const unsigned char *data, size_t size);

    size_t rawReceive(void *data, size_t size, bool nonBlocking = false);
    void rawTransmit(const void *data, size_t size);


    FileDescriptor m_conn;
    unsigned char m_packetBuffer[4096];
    size_t m_packetBufferUsed;
    bool m_doingAcks;
};

#endif
