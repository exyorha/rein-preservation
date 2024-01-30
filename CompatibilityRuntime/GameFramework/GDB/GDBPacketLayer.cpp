#include <GDB/GDBPacketLayer.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

#include <stdexcept>
#include <vector>
#include <cerrno>
#include <cstring>
#include <system_error>
#include <charconv>

GDBPacketLayer::GDBPacketLayer(const char *configuration) : m_packetBufferUsed(0), m_doingAcks(true) {

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

GDBPacketLayer::~GDBPacketLayer() = default;

bool GDBPacketLayer::getInterrupt() {
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

std::string GDBPacketLayer::getPacket() {

    while(true) {
        while(m_packetBufferUsed != 0) {
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
        if(m_packetBufferUsed == sizeof(m_packetBuffer))
            throw std::runtime_error("overly long packet received from GDB");

        m_packetBufferUsed += rawReceive(&m_packetBuffer[m_packetBufferUsed], sizeof(m_packetBuffer) - m_packetBufferUsed);
    }

    return {};
}

size_t GDBPacketLayer::rawReceive(void *data, size_t size, bool nonBlocking) {
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

void GDBPacketLayer::rawTransmit(const void *data, size_t size) {
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

void GDBPacketLayer::pollFor(int event) {
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

void GDBPacketLayer::packetBufferConsume(size_t size) {
    if(size > m_packetBufferUsed)
        throw std::logic_error("packetBufferConsume: overly long consume");

    if(m_packetBufferUsed > size)
        memmove(&m_packetBuffer[0], &m_packetBuffer[size], m_packetBufferUsed - size);

    m_packetBufferUsed -= size;
}

unsigned char GDBPacketLayer::packetChecksum(const unsigned char *data, size_t size) {
    unsigned char sum = 0;
    auto limit = data + size;
    while(data < limit) {
        sum += *data++;
    }

    return sum;
}

void GDBPacketLayer::sendPacket(const std::string &packet) {
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
