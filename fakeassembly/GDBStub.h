#ifndef GDB_STUB_H
#define GDB_STUB_H

#include "FileDescriptor.h"

#include <string>
#include <optional>

namespace Dynarmic {
    namespace A64 {
        class Jit;
    }
}

class GDBStub {
public:
    explicit GDBStub(const char *configuration, Dynarmic::A64::Jit &jit);
    ~GDBStub();

    GDBStub(const GDBStub &other) = delete;
    GDBStub &operator =(const GDBStub &other) = delete;

    void stopped(unsigned int signal);

    void beforeEnteringJIT();

    inline bool isSingleStepping() const {
        return m_singleSteppingCycles.has_value();
    }

    void stepped();

private:
    bool getInterrupt();
    std::string getPacket();
    void pollFor(int event);

    void packetBufferConsume(size_t bytes);
    static unsigned char packetChecksum(const unsigned char *data, size_t size);

    size_t rawReceive(void *data, size_t size, bool nonBlocking = false);
    void rawTransmit(const void *data, size_t size);

    bool processPacket(const std::string &packet);

    void sendPacket(const std::string &packet = {});

    void stopResponse();

    using TransferReadFunction = std::optional<std::string_view> (GDBStub::*)(const std::string_view &annex);

    std::optional<std::string_view> readTargetDescription(const std::string_view &annex);
    std::optional<std::string_view> readLibraries(const std::string_view &annex);
    std::optional<std::string_view> readExecFile(const std::string_view &annex);

    FileDescriptor m_conn;
    unsigned char m_packetBuffer[4096];
    size_t m_packetBufferUsed;
    bool m_doingAcks;
    bool m_stopped;
    unsigned int m_signal;
    Dynarmic::A64::Jit &m_jit;
    std::optional<size_t> m_singleSteppingCycles;
    std::optional<std::string> m_libraries;
    std::optional<std::string> m_execFilename;
};

#endif
