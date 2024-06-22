#include <cstdio>
#include <exception>
#include <charconv>
#include <thread>
#include <variant>

#include "Gameserver.h"
#include "ExitOnEof.h"

#include <getopt.h>
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#endif

template<typename T>
static T parseInteger(const std::string_view &string) {
    T value;

    auto result = std::from_chars(string.data(), string.data() + string.size(), value);
    if(result.ec != std::errc() || result.ptr != string.data() + string.size() || string.empty())
        throw std::runtime_error("bad number");

    return value;
}

static void setupListener(Gameserver &server, const std::string_view &address) {
    auto delim = address.rfind(':');

    std::string_view addressSection;
    std::string_view portSection;

    if(delim == std::string::npos) {
        addressSection = "0.0.0.0";
        portSection = address;
    } else {
        addressSection = address.substr(0, delim);
        portSection = address.substr(delim + 1);
    }

    auto portNumber = parseInteger<uint16_t>(portSection);

    printf("listening on %.*s port %u\n", static_cast<int>(addressSection.size()), addressSection.data(), portNumber);

    server.listen(std::string(addressSection).c_str(), portNumber);
}

static void setupListener(Gameserver &server, evutil_socket_t socket) {
    printf("listening on fd: %d\n", socket);

    evutil_make_socket_nonblocking(socket);
    evutil_make_socket_closeonexec(socket);

    server.acceptConnections(socket);
}

#ifdef _WIN32
static evutil_socket_t receiveWindowsSocket(HANDLE transferPipe) {

    WSAPROTOCOL_INFOW transferData;
    {
        struct PipeOwner {
            HANDLE pipe;

            explicit PipeOwner(HANDLE pipe) : pipe(pipe) {

            }

            ~PipeOwner() {
                if(pipe) {
                    CloseHandle(pipe);
                }
            }
        } owner(transferPipe);
        DWORD bytesRead;
        if(!ReadFile(owner.pipe, &transferData, sizeof(transferData), &bytesRead, nullptr) || bytesRead != sizeof(transferData))
            throw std::runtime_error("ReadFile has failed");
    }

    auto socket = WSASocket(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, &transferData, 0, 0);
    if(socket == INVALID_SOCKET)
        throw std::runtime_error("WSASocket has failed");

    return socket;
};

static void exitWithProcessThread(HANDLE handle) {
    WaitForSingleObject(handle, INFINITE);
    ExitProcess(0);
}

#endif

static std::filesystem::path pathFromArgument(const char *arg) {
#ifdef _WIN32
    std::string_view argStr(arg);
    return std::u8string_view(reinterpret_cast<const char8_t *>(argStr.data()), argStr.size());
#else
    return arg;
#endif
}

static int gameserver_main(int argc, char **argv) {

#ifdef _WIN32
      WSADATA wsa_data;
      WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif

#ifdef _WIN32
    setvbuf(stdout, nullptr, _IONBF, 256);
    setvbuf(stderr, nullptr, _IONBF, 256);
#else
    setlinebuf(stdout);
    setlinebuf(stderr);
#endif

    std::vector<std::variant<std::string, evutil_socket_t>> configuredListeners;
    std::optional<evutil_socket_t> exitOnEofFd;
#ifdef _WIN32
    std::optional<HANDLE> exitWithProcessHandle;
#endif

    static const struct option options[]{
        { .name = "master-database",     .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "individual-database", .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "octo-list",           .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "web-root",            .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "listen",              .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "accept-on-fd",        .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "exit-on-eof",         .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "accept-on-socket",    .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "exit-with-process",   .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { nullptr, 0, nullptr, 0 }
    };

    int longind;
    int result;

    const char *masterDatabasePath = nullptr;
    const char *individualDatabasePath = nullptr;
    const char *octoListPath = nullptr;
    const char *webroot = nullptr;

    while((result = getopt_long_only(argc, argv, "", options, &longind)) >= 0) {
        if(result == '?') {
            return 1;
        } else if(result != 0) {
            throw std::logic_error("unexpected return value from getopt_long_only: " + std::to_string(result));
        }

        switch(longind) {
            case 0:
                masterDatabasePath = optarg;
                break;

            case 1:
                individualDatabasePath = optarg;
                break;

            case 2:
                octoListPath = optarg;
                break;

            case 3:
                webroot = optarg;
                break;

            case 4:
                configuredListeners.emplace_back(optarg);
                break;

            case 5:
                configuredListeners.emplace_back(parseInteger<evutil_socket_t>(optarg));
                break;

            case 6:
                exitOnEofFd.emplace(parseInteger<evutil_socket_t>(optarg));
                break;

            case 7:
#ifdef _WIN32
                configuredListeners.emplace_back(receiveWindowsSocket(reinterpret_cast<HANDLE>(parseInteger<uintptr_t>(optarg))));
#else
                throw std::runtime_error("-accept-on-socket is a Windows-only option, use -accept-on-fd on Linux");
#endif
                break;

            case 8:
#ifdef _WIN32
                exitWithProcessHandle.emplace(reinterpret_cast<HANDLE>(parseInteger<uintptr_t>(optarg)));
#else
                throw std::runtime_error("-exit-with-process is a Windows-only option");
#endif
                break;

        }
    }

    if(!masterDatabasePath) {
        fprintf(stderr, "%s: -master-database must be specified\n", argv[0]);
        return 1;
    }

    if(!individualDatabasePath) {
        fprintf(stderr, "%s: -individual-database must be specified\n", argv[0]);
        return 1;
    }

    if(!octoListPath) {
        fprintf(stderr, "%s: -octo-list must be specified\n", argv[0]);
        return 1;
    }

    std::filesystem::path webRootPath;
    if(webroot) {
        webRootPath = pathFromArgument(webroot);
    } else {
        webRootPath = Gameserver::defaultWebRootPath();
    }

    Gameserver server(
        pathFromArgument(individualDatabasePath),
        pathFromArgument(masterDatabasePath),
        pathFromArgument(octoListPath),
        std::move(webRootPath));

    for(const auto &listener: configuredListeners) {
        std::visit([&server](const auto &value) {
            setupListener(server, value);
        }, listener);
    }

    std::optional<ExitOnEof> exitOnEof;

    if(exitOnEofFd.has_value()) {
        evutil_make_socket_nonblocking(*exitOnEofFd);
        evutil_make_socket_closeonexec(*exitOnEofFd);

        exitOnEof.emplace(&server.eventLoop(), *exitOnEofFd);
    }

#ifdef _WIN32
    if(exitWithProcessHandle.has_value()) {
        std::thread(exitWithProcessThread, *exitWithProcessHandle).detach();
    }
#endif

    server.wait();

    return 0;
}

#ifdef _WIN32

int wmain(int argc, wchar_t **argv) {
    std::vector<std::string> utf8Strings;
    utf8Strings.reserve(argc);

    for(int argno = 0; argno < argc; argno++) {
        std::wstring_view string(argv[argno]);
        std::string output;

        if(!string.empty()) {

            output.resize(string.size() * 4); // Worst case: every UTF-16 wchar_t generates 4 UTF-8 bytes (ignoring invalid 5, 6 byte UTF-8 sequences)

            int characters = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t *>(string.data()),
                                                (int)string.size(), &output[0], (int)output.size(), nullptr, nullptr);
            if (characters == 0)
                throw std::runtime_error("WideCharToMultiByte has failed");

            output.resize(characters);
        }

        utf8Strings.emplace_back(std::move(output));
    }

    std::vector<char *> utf8StringPointers;
    utf8StringPointers.reserve(utf8Strings.size());

    for(auto &string: utf8Strings) {
        utf8StringPointers.push_back(string.data());
    }

    utf8StringPointers.push_back(nullptr);

    return gameserver_main(argc, utf8StringPointers.data());
}

#else

int main(int argc, char **argv) {
    return gameserver_main(argc, argv);
}

#endif

