#include "GameServerInterface.h"
#include "ProcessCommandLine.h"
#include "CommonUtilities.h"
#include "Octo.h"
#include "OctoContentStorage.h"

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#endif

#include <stdexcept>
#include <system_error>
#include <cstring>
#include <sstream>

static std::string GameServerEndpoint;

namespace {
#if defined(_WIN32)
    using SocketType = SOCKET;
    static constexpr SocketType InvalidSocketValue = INVALID_SOCKET;
    using SocketNameLength = int;

    static inline bool badSocket(SOCKET result) {
        return result == INVALID_SOCKET;
    }

    static inline bool badCall(int result) {
        return result == SOCKET_ERROR;
    }

    [[noreturn]] static void raiseSocketError() {
        auto error = WSAGetLastError();
        throw std::runtime_error("socket error: " + std::to_string(error));
    }

#else
    using SocketType = int;
    static constexpr SocketType InvalidSocketValue = -1;
    using SocketNameLength = socklen_t;

    static inline bool badSocket(int result) {
        return result < 0;
    }

    static inline bool badCall(int result) {
        return result < 0;
    }

    [[noreturn]] static void raiseSocketError() {
        throw std::system_error(errno, std::generic_category());
    }

    static inline void closesocket(int fd) {
        ::close(fd);
    }
#endif
}

static std::string spawnGameServer() {
    printf("Spawning the game server\n");

    struct SocketKeeper {
        SocketType handle = InvalidSocketValue;

        ~SocketKeeper() {
            if(!badSocket(handle)) {
                closesocket(handle);
            }
        }
    } listeningSocket;

    listeningSocket.handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(badSocket(listeningSocket.handle))
        raiseSocketError();

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = 0;
    if(badCall(bind(listeningSocket.handle, reinterpret_cast<const struct sockaddr *>(&address), sizeof(address))))
        raiseSocketError();

    SocketNameLength len = sizeof(address);
    if(badCall(getsockname(listeningSocket.handle, reinterpret_cast<struct sockaddr *>(&address), &len)))
        raiseSocketError();

    char textAddress[INET_ADDRSTRLEN];
    auto result = inet_ntop(address.sin_family, &address.sin_addr, textAddress, sizeof(textAddress));
    if(!result)
        raiseSocketError();

    std::stringstream endpointStream;
    endpointStream << "http://" << textAddress << ":" << ntohs(address.sin_port);

    printf("The server will be at %s\n", endpointStream.str().c_str());

    if(badCall(listen(listeningSocket.handle, SOMAXCONN)))
        raiseSocketError();

#ifdef _WIN32
    auto gameServerName = "GameServer.exe";
#else
    auto gameServerName = "GameServer";
#endif

    using namespace std::string_view_literals;

    ProcessCommandLine cmdline(executableDirectory() / gameServerName);
    cmdline.addArgument("-master-database"sv);
    cmdline.addArgument(contentStorageInstance->masterDatabase());
    cmdline.addArgument("-octo-list"sv);
    cmdline.addArgument(contentStorageInstance->octoListFile());
    cmdline.addArgument("-individual-database"sv);
    cmdline.addArgument(getGamePersistentDataPath() / "individual.db");

#ifdef _WIN32
    /*
     * Passing a socket to a process on Windows is an involved process. Socket
     * handles cannot be safely inherited, instead they must be passed to the
     * recipient process with an Winsock API call, and the resulting structure
     * needs to be sent over. Passing a socket handle requires the target
     * process ID, so we need to start the process first to get the structure.
     * So we create a pipe and pass the structure via that pipe.
     */
    struct HandleOwner {
        HANDLE handle = nullptr;

        ~HandleOwner() {
            if(handle) {
                CloseHandle(handle);
            }
        }
    };

    HandleOwner readPipe, writePipe;
    if(!CreatePipe(&readPipe.handle, &writePipe.handle, nullptr, 0))
        throw std::runtime_error("CreatePipe has failed");

    /*
     * Replace the read pipe handle with an inheritable duplicate.
     */
    if(!DuplicateHandle(GetCurrentProcess(), readPipe.handle, GetCurrentProcess(), &readPipe.handle,
        0, TRUE, DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS))
        throw std::runtime_error("DuplicateHandle has failed");

    cmdline.addArgument("-accept-on-socket"sv);
    cmdline.addArgument(std::string_view(std::to_string(reinterpret_cast<uintptr_t>(readPipe.handle))));

    /*
     * We pass an inheritable handle of this process to the game server so it
     * can wait on it and exit when the game process exits.
     */
    HandleOwner thisProcessHandle;
    if(!DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(), &thisProcessHandle.handle,
        SYNCHRONIZE, TRUE, 0))
        throw std::runtime_error("DuplicateHandle has failed");

    cmdline.addArgument("-exit-with-process"sv);
    cmdline.addArgument(std::string_view(std::to_string(reinterpret_cast<uintptr_t>(thisProcessHandle.handle))));

#else
    /*
     * On Linux, we simply pass the file descriptor of the socket.
     */
    cmdline.addArgument("-accept-on-fd"sv);
    cmdline.addArgument(std::string_view(std::to_string(listeningSocket.handle)));

    /*
     * On Linux, we create a dummy pipe and 'leak' the write side fd of it in
     * the game process. This pipe will close when the game process exits. The
     * server will execute a read on the pipe, and when the pipe closes, it
     * will get the EOF of it, and exit.
     */
    struct FDOwner {
        int fd = -1;

        ~FDOwner() {
            if(fd >= 0)
                close(fd);
        }
    } readSide, writeSide;

    int rawfds[2];
    if(pipe2(rawfds, O_CLOEXEC) < 0)
        throw std::system_error(errno, std::generic_category());

    readSide.fd = rawfds[0];
    writeSide.fd = rawfds[1];

    /*
     * Close-on-exec needs to be cleared on the read side.
     */
    fcntl(readSide.fd, F_SETFD, fcntl(readSide.fd, F_GETFD) & ~FD_CLOEXEC);

    cmdline.addArgument("-exit-on-eof"sv);
    cmdline.addArgument(std::string_view(std::to_string(readSide.fd)));
#endif

    auto pid = cmdline.startProcess();
    printf("The game server has been started with the PID %u\n", static_cast<uint32_t>(pid));

#ifdef _WIN32

    /*
     * Now that we have the process ID, we can finish passing the socket.
     */

    WSAPROTOCOL_INFOW protocol;
    if(badCall(WSADuplicateSocket(listeningSocket.handle, pid, &protocol)))
        raiseSocketError();

    DWORD bytesWritten;
    if(!WriteFile(writePipe.handle, &protocol, sizeof(protocol), &bytesWritten, nullptr) || bytesWritten != sizeof(protocol))
        throw std::runtime_error("WriteFile has failed");

#else
    /*
     * Leak the write side of the pipe, as described above.
     */
    writeSide.fd = -1;
#endif

    return endpointStream.str();
}

void setGameServerEndpoint(std::string &&endpoint) {
    GameServerEndpoint = std::move(endpoint);
}

const std::string &getGameServerEndpoint() {
    if(GameServerEndpoint.empty()) {
        setGameServerEndpoint(spawnGameServer());
    }

    return GameServerEndpoint;
}
