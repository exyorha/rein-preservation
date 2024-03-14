#include <cstdio>
#include <exception>
#include <charconv>

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

int main(int argc, char **argv) {

#ifdef _WIN32
      WSADATA wsa_data;
      WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif

    std::vector<std::variant<std::string, evutil_socket_t>> configuredListeners;
    std::optional<evutil_socket_t> exitOnEofFd;

    static const struct option options[]{
        { .name = "master-database",     .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "individual-database", .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "octo-list",           .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "web-root",            .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "listen",              .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "accept-on-fd",        .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "exit-on-eof",         .has_arg = required_argument, .flag = nullptr, .val = 0 },
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
        webRootPath = webroot;
    } else {
        webRootPath = Gameserver::defaultWebRootPath();
    }

    Gameserver server(individualDatabasePath, masterDatabasePath, octoListPath, webRootPath);

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

    server.wait();

    return 0;
}
