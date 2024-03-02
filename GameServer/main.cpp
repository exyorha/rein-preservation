#include <cstdio>
#include <exception>

#include "Gameserver.h"

#include <getopt.h>

#ifdef _WIN32
#include <winsock2.h>
#endif

int main(int argc, char **argv) {

#ifdef _WIN32
      WSADATA wsa_data;
      WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif

    static const struct option options[]{
        { .name = "master-database",     .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { .name = "individual-database", .has_arg = required_argument, .flag = nullptr, .val = 0 },
        { nullptr, 0, nullptr, 0 }
    };

    int longind;
    int result;

    const char *masterDatabasePath = nullptr;
    const char *individualDatabasePath = nullptr;

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

    Gameserver server(individualDatabasePath, masterDatabasePath);

    server.listen("0.0.0.0", 8087);

    server.wait();

    return 0;
}
