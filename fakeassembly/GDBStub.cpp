#include "GDBStub.h"
#include "FileDescriptor.h"
#include "GDBPacketParser.h"
#include "GDBPacketFormatter.h"
#include "DebugMemoryAccess.h"
#include "support.h"
#include "Image.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <cstring>
#include <system_error>
#include <charconv>
#include <vector>
#include <sstream>
#include <signal.h>

#include <dynarmic/interface/A64/a64.h>

GDBStub::GDBStub(const char *configuration, Dynarmic::A64::Jit &jit) : m_packetBufferUsed(0), m_doingAcks(true), m_signal(SIGTRAP),
    m_jit(jit), m_stopped(true) {

    int rawfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
    if(rawfd < 0)
        throw std::system_error(errno, std::generic_category());

    FileDescriptor listeningSocket(rawfd);

    int val = 1;
    if(setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
        throw std::system_error(errno, std::generic_category());

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(atoi(configuration));

    if(bind(listeningSocket, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0)
        throw std::system_error(errno, std::generic_category());

    socklen_t len = sizeof(addr);
    if(getsockname(listeningSocket, reinterpret_cast<struct sockaddr *>(&addr), &len) < 0)
        throw std::system_error(errno, std::generic_category());

    if(listen(listeningSocket, 1) < 0)
        throw std::system_error(errno, std::generic_category());

    printf("\n"
           "Awaiting an aarch64 GDB connection at TCP port %u for the ARM code debugging.\n"
           "The execution will not start until a debugger is attached. If you've intended\n"
           "to run the application without a debugger, start it with the ARM debugging\n"
           "mode disabled.\n"
           "\n",
           ntohs(addr.sin_port));
    fflush(stdout);

    do {
        do {
            rawfd = accept4(listeningSocket, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
        } while(rawfd < 0 && errno == EINTR);

        if(rawfd < 0) {
            if(errno != ECONNABORTED)
                throw std::system_error(errno, std::generic_category());
        }
    } while(rawfd < 0);

    m_conn = FileDescriptor(rawfd);
    listeningSocket.reset();

    val = 1;
    if(setsockopt(m_conn, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)))
        throw std::system_error(errno, std::generic_category());


    printf(
        "\n"
        "The debugger connection has been accepted.\n"
        "\n");

    fflush(stdout);
}

GDBStub::~GDBStub() = default;

void GDBStub::beforeEnteringJIT() {

    do {

        if(m_stopped) {
            printf("GDBStub: beforeEnteringJIT on a stopped target, processing commands\n");
            bool resume;
            do {
                auto packet = getPacket();

                resume = processPacket(packet);
                if(resume) {
                    printf("GDBStub: resuming the JIT execuion\n");

                    m_stopped = false;
                }
            } while(!resume);
        } else {
            if(getInterrupt()) {
                printf("GDBStub: interrupt received, stopping the target\n");
                stopped(SIGINT);
            }
        }

    } while(m_stopped);
}

void GDBStub::stepped() {
    if(m_singleSteppingCycles.has_value()) {
        if(*m_singleSteppingCycles != 0)
            *m_singleSteppingCycles -= 1;

        if(*m_singleSteppingCycles == 0) {
            stopped(SIGTRAP);
        }
    }
}

void GDBStub::stopped(unsigned int signal) {
    printf("GDBStub: stopping on signal %u\n", signal);

    if(m_stopped) {
        throw std::runtime_error("GDBStub::stopped: the target is already stopped");
    }

    m_signal = signal;
    m_stopped = true;
    m_singleSteppingCycles.reset();

    stopResponse();
}

bool GDBStub::getInterrupt() {
    while(true) {
        while(m_packetBufferUsed != 0) {
            auto leading = m_packetBuffer[0];
            if(leading == 0x03) {
                packetBufferConsume(1);
                return true;
            } else {
                printf("non-empty packet buffer on a stopped target: '%.*s'\n", static_cast<int>(m_packetBufferUsed),
                    reinterpret_cast<char *>(m_packetBuffer));

                throw std::runtime_error("non-empty packet buffer on a stopped target");
            }
        }

        auto bytesRead = rawReceive(&m_packetBuffer[m_packetBufferUsed], sizeof(m_packetBuffer) - m_packetBufferUsed, true);

        m_packetBufferUsed += bytesRead;
        if(bytesRead == 0)
            return false;
    }

}

std::string GDBStub::getPacket() {

    while(true) {
        while(m_packetBufferUsed != 0) {
            printf("Accumulated GDB packet data: '%.*s'\n", static_cast<int>(m_packetBufferUsed),
                reinterpret_cast<char *>(m_packetBuffer));


            auto leading = m_packetBuffer[0];
            if(leading == '+' || leading == '-') {
                printf("Received a GDB ACK or NAK ('%c') when none was expected\n", leading);

                packetBufferConsume(1);
            } else if(leading == '$') {

                auto end = static_cast<const unsigned char *>(memchr(m_packetBuffer, '#', m_packetBufferUsed));
                /*
                 * No packet terminator character, or the checksum wouldn't fit.
                 */
                if(end == nullptr || m_packetBuffer + m_packetBufferUsed - end < 2) {
                    break;
                }

                auto packetData = &m_packetBuffer[1];
                auto packetDataLength = end - m_packetBuffer - 1;
                auto fullPacketLength = end + 3 - m_packetBuffer;

                /*
                 * Calculate the packet checksum.
                 */
                auto checksum = packetChecksum(packetData, packetDataLength);

                uint8_t inPacketChecksum;
                auto checksumCharacters = reinterpret_cast<const char *>(end + 1);
                auto result = std::from_chars(
                    checksumCharacters,
                    checksumCharacters + 2,
                    inPacketChecksum, 16);
                if(result.ec != std::errc() || result.ptr != checksumCharacters + 2 || inPacketChecksum != checksum) {
                    if(m_doingAcks) {
                        printf("Checksum error in a GDB packet\n");

                        char ack = '-';
                        rawTransmit(&ack, sizeof(ack));
                        packetBufferConsume(fullPacketLength);
                        continue;
                    } else {
                        throw std::runtime_error("checksum error in a GDB packet in the no-ack mode");
                    }
                }

                /*
                 * Unescape the binary data;
                 */

                for(unsigned char *ptr = packetData; ptr < packetData + packetDataLength; ptr++) {
                    if(*ptr == '}') {
                        /*
                         * Escape character.
                         */
                        auto tail = packetData + packetDataLength - ptr;
                        if(tail > 0) {
                            memmove(ptr, ptr + 1, tail - 1);
                            *ptr ^= 0x20;
                            packetDataLength--;
                        }
                    }
                }


                printf("< '%.*s'\n", static_cast<int>(packetDataLength), packetData);

                auto packet = std::string(
                    reinterpret_cast<char *>(packetData),
                    packetDataLength);


                packetBufferConsume(fullPacketLength);

                if(m_doingAcks) {
                    char ack = '+';
                    rawTransmit(&ack, sizeof(ack));
                }

                return packet;

            } else {
                throw std::runtime_error("garbage in the GDB connection or loss of synchronization");
            }
        }

        /*
         * Not enough data to assemble a packet; receive more.
         */

        printf("Receiving more data.\n");

        if(m_packetBufferUsed == sizeof(m_packetBuffer))
            throw std::runtime_error("overly long packet received from GDB");

        m_packetBufferUsed += rawReceive(&m_packetBuffer[m_packetBufferUsed], sizeof(m_packetBuffer) - m_packetBufferUsed);
    }

    return {};
}

size_t GDBStub::rawReceive(void *data, size_t size, bool nonBlocking) {
    ssize_t bytesRead;
    do {
        do {
            bytesRead = recv(m_conn, data, size, 0);
        } while(bytesRead < 0 && errno == EINTR);

        if(bytesRead < 0) {
            if(errno == EWOULDBLOCK) {
                if(nonBlocking)
                    return 0;
                else
                    pollFor(POLLIN);
            } else {
                throw std::system_error(errno, std::generic_category());
            }
        }
    } while(bytesRead < 0);

    if(bytesRead == 0) {
        throw std::runtime_error("the connection to the debugger has been closed");
    }

    return static_cast<size_t>(bytesRead);
}

void GDBStub::pollFor(int event) {
    struct pollfd pfd;
    pfd.fd = m_conn;
    pfd.events = event;

    int result;
    do {
        do {
            result = poll(&pfd, 1, -1);
        } while(result < 0 && errno == EINTR);

        if(result < 0)
            throw std::system_error(errno, std::generic_category());

        if(pfd.revents & (POLLERR | POLLNVAL))
            throw std::runtime_error("error condition on the connection file descriptor");
    } while(result == 0 || !(pfd.revents & event));
}

void GDBStub::rawTransmit(const void *data, size_t size) {
    auto bytes = static_cast<const unsigned char *>(data);
    while(size > 0) {
        ssize_t result;
        do {
            result = send(m_conn, bytes, size, 0);
        } while(result < 0 && errno == EINTR);

        if(result < 0) {
            if(errno == EWOULDBLOCK) {
                pollFor(POLLOUT);
            } else {
                throw std::system_error(errno, std::generic_category());
            }
        } else {
            bytes += result;
            size -= result;
        }
    }
}

void GDBStub::packetBufferConsume(size_t size) {
    if(size > m_packetBufferUsed)
        throw std::logic_error("packetBufferConsume: overly long consume");

    if(m_packetBufferUsed > size)
        memmove(&m_packetBuffer[0], &m_packetBuffer[size], m_packetBufferUsed - size);

    m_packetBufferUsed -= size;
}

unsigned char GDBStub::packetChecksum(const unsigned char *data, size_t size) {
    unsigned char sum = 0;
    auto limit = data + size;
    while(data < limit) {
        sum += *data++;
    }

    return sum;
}

void GDBStub::stopResponse() {
    std::stringstream stopReply;
    stopReply << "S" << std::hex;
    stopReply.width(2);
    stopReply.fill('0');
    stopReply << m_signal;

    sendPacket(stopReply.str());
}

bool GDBStub::processPacket(const std::string &packet) {
    printf("Processing a GDB packet: '%s'\n", packet.c_str());

    GDBPacketParser parser(packet);
    if(parser.atEnd()) {
        printf("Empty GDB packet received");
        sendPacket();
        return false;
    }

    switch(parser.getCharacter()) {
        case '?':
            stopResponse();
            break;

        case 'g':
        {
            /*
             * Read all registers. The register order is:
             * - all GPRs
             * - SP,
             * - PC,
             * - PSTATE,
             * - all vector registers,
             * - FPSR,
             * - FPCR
             */
            std::vector<unsigned char> packetData;
            packetData.reserve((31 + 2) * 8 + 4 + 32 * 16);
            auto gprs = m_jit.GetRegisters();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(gprs.data()),
                              reinterpret_cast<unsigned char *>(gprs.data()) + sizeof(gprs));

            uint64_t sp = m_jit.GetSP();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&sp),
                              reinterpret_cast<unsigned char *>(&sp) + sizeof(sp));


            uint64_t pc = m_jit.GetPC();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&pc),
                              reinterpret_cast<unsigned char *>(&pc) + sizeof(pc));

            uint32_t pstate = m_jit.GetPstate();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&pstate),
                              reinterpret_cast<unsigned char *>(&pstate) + sizeof(pstate));

            auto vectors = m_jit.GetVectors();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&vectors),
                              reinterpret_cast<unsigned char *>(&vectors) + sizeof(vectors));

            auto fpsr = m_jit.GetFpsr();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&fpsr),
                              reinterpret_cast<unsigned char *>(&fpsr) + sizeof(fpsr));

            auto fpcr = m_jit.GetFpcr();
            packetData.insert(packetData.end(),
                              reinterpret_cast<unsigned char *>(&fpcr),
                              reinterpret_cast<unsigned char *>(&fpcr) + sizeof(fpcr));

            GDBPacketFormatter formatter;
            formatter.hex(packetData.data(), packetData.size());

            sendPacket(formatter.string());

            break;
        }

        case 'q':
        {
            auto query = parser.getColonDelimited();

            if(query == "Supported") {
                std::stringstream supported;
                supported << "qSupported:PacketSize=" << std::hex << sizeof(m_packetBuffer) << ";QStartNoAckMode+;qXfer:features:read+;qXfer:libraries:read+;qXfer:exec-file:read+";

                sendPacket(supported.str());

            } else if(query == "Xfer") {
                auto object = parser.getColonDelimited();
                auto command = parser.getColonDelimited();

                if(command == "read") {
                    TransferReadFunction reader;

                    if(object == "features") {
                        reader = &GDBStub::readTargetDescription;

                    } else if(object == "libraries") {
                        reader = &GDBStub::readLibraries;

                    } else if(object == "exec-file") {
                        reader = &GDBStub::readExecFile;

                    } else {
                        // Wrong *object* should give an empty response.
                        printf("Unknown GDB Xfer object on read: '%s'\n", packet.c_str());
                        sendPacket();
                        return false;
                    }

                    auto annex = parser.getColonDelimited();

                    auto offset = GDBPacketParser::parseInteger<size_t>(parser.getCommaDelimited());
                    auto length = GDBPacketParser::parseInteger<size_t>(parser.getRest());

                    auto data = (this->*reader)(annex);
                    if(!data.has_value()) {
                        sendPacket("E00");
                        return false;
                    }


                    size_t bytesToRead;
                    if(offset > data->size())
                        bytesToRead = 0;
                    else
                        bytesToRead = std::min<size_t>(length, data->size() - offset);

                    std::string response;
                    response.reserve(1 + bytesToRead);
                    if(offset + bytesToRead >= data->size())
                        response.push_back('l');
                    else
                        response.push_back('m');

                    response.append(data->substr(offset, bytesToRead));
                    sendPacket(response);

                } else {
                    printf("Unknown GDB Xfer command: '%s'\n", packet.c_str());
                    sendPacket();
                }
            } else if(query == "Offsets") {
                std::stringstream offsets;
                offsets << "TextSeg=";
                offsets << std::hex << Image::get_il2cpp_image_debugger()->displacement();
                sendPacket(offsets.str());

            } else {
                printf("Unknown GDB query: '%s'\n", packet.c_str());
                sendPacket();
            }

            break;
        }

        case 'Q':
        {
            auto set = parser.getColonDelimited();

            if(set == "StartNoAckMode") {
                sendPacket("OK");
                m_doingAcks = false;
            } else {
                printf("Unknown GDB set: '%s'\n", packet.c_str());
                sendPacket();
            }
            break;
        }

        case 'm':
        {
            auto address = GDBPacketParser::parseInteger<uintptr_t>(parser.getCommaDelimited());
            auto length = GDBPacketParser::parseInteger<size_t>(parser.getRest());

            std::vector<unsigned char> buf(length);
            auto result = debugMemoryRead(address, buf.data(), length);

            GDBPacketFormatter formatter;
            if(!result)
                formatter.error(EFAULT);
            else
                formatter.hex(buf.data(), buf.size());

            sendPacket(formatter.string());

            break;
        }

        case 'M':
        {
            auto address = GDBPacketParser::parseInteger<uintptr_t>(parser.getCommaDelimited());
            auto length = GDBPacketParser::parseInteger<size_t>(parser.getColonDelimited());

            printf("parsing\n");
            auto bytes = GDBPacketParser::parseHex(parser.getRest());

            GDBPacketFormatter formatter;

            if(bytes.size() != length) {
                formatter.error(EINVAL);
            } else {

            printf("writing\n");
                auto result = debugMemoryWrite(address, bytes.data(), bytes.size());

            printf("written , %d\n", result);

                if(!result)
                    formatter.error(EFAULT);
                else
                    formatter.text("OK");
            }

            sendPacket(formatter.string());
            break;
        }

        case 'c':
        {
            if(!parser.atEnd()) {
                m_jit.SetPC(GDBPacketParser::parseInteger<uint64_t>(parser.getRest()));
            }

            return true;
        }

        case 'i':
        {
            if(!parser.atEnd()) {
                m_jit.SetPC(GDBPacketParser::parseInteger<uint64_t>(parser.getCommaDelimited()));
            }

            if(parser.atEnd()) {
                m_singleSteppingCycles.emplace(1);
            } else {
                m_singleSteppingCycles.emplace(GDBPacketParser::parseInteger<size_t>(parser.getRest()));
            }

            return true;
        }

        case 's':
        {
            if(!parser.atEnd()) {
                m_jit.SetPC(GDBPacketParser::parseInteger<uint64_t>(parser.getCommaDelimited()));
            }

            m_singleSteppingCycles.emplace(1);
            return true;
        }

        case 'k':
            sendPacket();
            panic("The debugging session has been killed by the debugger.\n");
            break;

        default:
            printf("Unknown GDB packet received: '%s'\n", packet.c_str());
            sendPacket();
            break;
    }

    return false;
}

void GDBStub::sendPacket(const std::string &packet) {
    printf("> '%s'\n", packet.c_str());

    std::vector<unsigned char> composedBuffer;
    composedBuffer.reserve(packet.size() + 4); // sufficient if there's nothing to escape, which is the most common case.

    composedBuffer.push_back('$');

    for(char ch: packet) {
        auto uch = static_cast<unsigned char>(ch);

        if(uch == 0x23 || uch == 0x24 || uch == 0x7D || uch == 0x2A) {
            composedBuffer.push_back(0x7D);
            composedBuffer.push_back(uch ^ 0x20);
        } else {
            composedBuffer.push_back(uch);
        }
    }

    composedBuffer.push_back('#');

    auto checksum = packetChecksum(composedBuffer.data() + 1, composedBuffer.size() - 2);

    char checksumChars[2];
    auto result = std::to_chars(checksumChars, checksumChars + sizeof(checksumChars), checksum, 16);
    if(result.ec != std::errc())
        throw std::runtime_error("failed to convert the checksum");

    auto checksumEnd = result.ptr;
    size_t checksumLength = checksumEnd - checksumChars;

    size_t padded = checksumLength;
    while(padded < sizeof(checksumChars)) {
        composedBuffer.push_back('0');
        padded++;
    }

    composedBuffer.insert(composedBuffer.end(),
                          reinterpret_cast<unsigned char *>(checksumChars),
                          reinterpret_cast<unsigned char *>(checksumEnd));

    char ack;

    do {
        rawTransmit(composedBuffer.data(), composedBuffer.size());

        if(!m_doingAcks)
            break;

        rawReceive(&ack, sizeof(ack));

        if(ack == '+')
            break;
        else if(ack != '-') {
            throw std::runtime_error("GDB sent something that's neither ACK or NAK in response to a packet: " + std::to_string(
                static_cast<unsigned int>(ack)));
        }
    } while(ack == '-');
}

std::optional<std::string_view> GDBStub::readTargetDescription(const std::string_view &annex) {
    static const char *targetDescriptionXML = R"XML(<?xml version="1.0"?>
<!DOCTYPE target SYSTEM "gdb-target.dtd">
<target version="1.0">
    <architecture>aarch64</architecture>
    <feature name="org.gnu.gdb.aarch64.core" />
    <feature name="org.gnu.gdb.aarch64.fpu" />
</target>
)XML";

    if(annex == "target.xml")
        return targetDescriptionXML;
    else
        return {};
}

std::optional<std::string_view> GDBStub::readLibraries(const std::string_view &annex) {
    if(!annex.empty())
        return {};

    if(!m_libraries.has_value()) {

        std::stringstream libraryDesc;
        auto il2cpp = Image::get_il2cpp_image_debugger();
        libraryDesc <<
            "<library-list>\n"
                "<library name=\"" << il2cpp->path() << "\" />\n"
                    "<segment address=\">0x" << std::hex << il2cpp->displacement() << "\" />\n"
                "</library>\n"
            "</library-list>\n";
        m_libraries = std::move(libraryDesc.str());
    }

    return *m_libraries;
}

std::optional<std::string_view> GDBStub::readExecFile(const std::string_view &annex) {
    if(!annex.empty())
        return {};

    if(!m_execFilename.has_value()) {

        m_execFilename = Image::get_il2cpp_image_debugger()->path();
    }

    return *m_execFilename;
}
